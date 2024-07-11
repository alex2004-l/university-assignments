#include <cmath>
#include <unordered_map>
#include <climits>
#include <algorithm>
#include <sstream>

#include "common.hpp"

#define INT "INT"
#define SHORT_REAL "SHORT_REAL"
#define FLOAT "FLOAT"
#define STRING "STRING"
#define MAX_BUFF_SIZE 1501
#define MAX_UDP_SIZE  1552
#define ID_SIZE         11

/* Verifica daca s-a primti de la stdin string-ul de iesire */
bool check_exit() {
    char input[128];
    size_t input_len;

    fgets(input, sizeof(input), stdin);
    input_len = strlen(input);

    while (isspace(input[input_len - 1])) {
        input[input_len - 1] = '\0';
        input_len --;
    }

    if (!strcmp(input, EXIT_STR)) {
        return true;
    }
    return false;
}

/* Decodeaza un mesaj primit de la un client UDP si salveaza
* datele in headerul si payloadul primite ca argumente*/
bool decode_message(struct udp_msg *message, struct tcp_msg_hdr *message_header, char **payload) {
    int number;
    float short_real_number;
    double real_number;
    uint8_t power;
    bool has_terminator;

    switch (message->type) {
        case 0:
            number = ntohl((*(uint32_t *)(message->value + sizeof(uint8_t))));

            /* Verificare daca byte-ul de semn e setat */
            if (message->value[0]) {
                number = number * (-1);
            }

            sprintf(*payload, "%d", number);
            strcpy(message_header->type, INT);

            break;
        case 1:
            short_real_number = ntohs((*(uint16_t *)(message->value)));
            short_real_number/=100;

            sprintf(*payload, "%.2f", short_real_number);
            strcpy(message_header->type, SHORT_REAL);

            break;
        case 2:
            real_number = ntohl((*(uint32_t *)(message->value + sizeof(uint8_t))));
            power = (*(uint8_t *)(message->value + sizeof(uint8_t) + sizeof(uint32_t)));
            for (int i = 0 ; i < power; ++i) real_number/=10;

            /* Verificare daca byte-ul de semn e setat */
            if(message->value[0]) {
                real_number = real_number * (-1);
            }

            sprintf(*payload, "%.*f", power, real_number);
            strcpy(message_header->type, FLOAT);

            break;
        case 3:
            /* verificare daca mesajul are terminator */ 
            has_terminator = false;
            for(int i = 0; i < MAX_BUFF_SIZE; ++i) {
                if (message->value[i] == '\0') {
                    has_terminator = true;
                    break;
                }
            }

            if (!has_terminator) {
                message->value[MAX_BUFF_SIZE - 1] = '\0';
            }

            strcpy(*payload, message->value);
            strcpy(message_header->type, STRING);

            break;
        default:
            return false;
    }

    message_header->len = strlen(*payload) + 1;
    strncpy(message_header->topic, message->topic, 50);

    /* Garanteaza ca exista terminator indiferent de topicul primit */
    message_header->topic[50] = '\0';

    return true;
}

/* Adauga un nou FD in vectorul folosit la multiplexare */
void add_new_poll_fd(vector<struct pollfd> &fds, int new_fd) {
    struct pollfd new_poll_entry;
    memset((void *) &new_poll_entry, 0, sizeof(struct pollfd));

    new_poll_entry.fd = new_fd;
    new_poll_entry.events = POLLIN;

    fds.push_back(new_poll_entry);
}

/* Functie care imparte un string in mai multe stringuri in functie de
 * un delimitator */
vector<string> split(const string &str, char delim) {
    vector<string> tokens;
    string token;
    istringstream tokenStream(str);
    while (getline(tokenStream, token, delim)) {
        tokens.push_back(token);
    }
    return tokens;
}

/* Verifica daca primul topic primit corespunde cu al doilea
 * SUnt luate in considerare si optiunile cu wildcard */
bool check_topics(char buf[], string topic2) {
    string topic1(buf);

    /* Se impart topicurile in functie de delimitatorul '/' */
    vector<string> t1 = split(topic1, '/');
    vector<string> t2 = split(topic2, '/');

    unsigned long int t1_size = t1.size(), t2_size = t2.size();

    for (unsigned long int i = 0, j = 0; i < t1_size && j < t2_size; ) {
        if (t2[j] == "+") {
            ++i;
            ++j;
        } else if (t2[j] == "*") {
            if (j + 1 < t2_size) {
                ++j;
                while (i < t1_size && t2[j] != t1[i]) {
                    ++i;
                }
            } else {
                return true;
            }
        } else if (t2[j] == t1[i]) {
            ++i;
            ++j;
        } else return false;

        /* Daca unul din vectori se termina inaintea celuilalt,
         * topicurile nu sunt compatibile */
        if (i >= t1_size && j < t2_size) {
            return false;
        }
        if (i < t1_size && j >= t2_size) {
            return false;
        }
    }

    return true;
}


/* Returneaza id-ul clientului care este conectat la socketul cu fd sockfd */
string get_client_id(unordered_map<string, int> &online, int sockfd) {
    auto it = online.begin();

    while (it != online.end()) {
        if (it->second == sockfd)
            return it->first;
    }
    return NULL;
}

/* Daca primim un mesaj de subscribe/unsubscribe se actualizeaza lista
 * de topicuri aferente clientului care a trimis mesajul */
void decode_client_request(string id, struct tcp_msg_hdr message,
        unordered_map<string, vector<string>> &clients) {
    string topic(message.topic);
    vector<string> topics = clients.at(id);

    if (!strcmp(message.type, "SUB")) {
        auto find_check = find(topics.begin(), topics.end(), topic);
        if (find_check == topics.end()) {
            topics.push_back(topic);
        }

    }

    if (!strcmp(message.type, "UNSUB")) {
        for (auto it = topics.begin(); it != topics.end();) {
            if (*it == topic) {
                it = topics.erase(it);
            } else {
                ++it;
            }
        }
    }

    clients.insert_or_assign(id, topics);
}


/* Pentru clientii online, se cauta in lista topicurilor la care acestia s-au abonat,
 * si daca se face match pe vreunul, se trimite mesajul catre client */
void send_subscribers(struct tcp_msg_hdr header, char *payload, unordered_map<string,
        vector<string>> clients, unordered_map<string, int> online) {

    for (auto it = online.begin(); it != online.end(); ++it) {
        vector<string> topics = clients.at(it->first);

        bool check = false;

        for (unsigned long int i = 0; i < topics.size() && !check; ++i) {
            check = check_topics(header.topic, topics[i]);
        }

        /* Daca mesajul se potriveste cu cel putin un topic, se trimite catre client */
        if (check) {
            /* Maai intai se trimite header-ul, dupa care se trimite payload-ul */
            send_all(it->second, (void *) &header, sizeof(struct tcp_msg_hdr));
            send_all(it->second, (void *) payload, strlen(payload) + 1);
        }
    }
}


/* Functie pentru primirea mesajelor de la clientii UDP si redirectionarea
 * acestora catre clientii TCP*/
void receive_message_udp_client(int socket_udp, unordered_map<string,
        vector<string>> clients, unordered_map<string, int> online) {

    char buffer[MAX_UDP_SIZE];

    struct sockaddr_in udp_client;
    socklen_t udp_len = sizeof(udp_client);

    /* Se primeste un mesaj pe socketul UDP */
    int ret = recvfrom(socket_udp, buffer, MAX_UDP_SIZE, 0,
                        (struct sockaddr *)&udp_client, &udp_len);
    DIE(ret < 0, "Error from recvfrom.\n");

    struct udp_msg *message = (struct udp_msg *) buffer;
    
    /* Se construieste mesajul pentru clientii TCP */
    struct tcp_msg_hdr message_header;

    message_header.ip_addr = udp_client.sin_addr;
    message_header.port = ntohs(udp_client.sin_port);

    char *buf = (char *) malloc(MAX_BUFF_SIZE);
    bool valid_message = decode_message(message, &message_header, &buf);

    /* Se trimite mesajul construit catre clientii online abonati */
    if (valid_message)
        send_subscribers(message_header, buf, clients, online);

    free(buf);
}


/* Functie unde se accepta conexiunile clientilor noi neduplicati */
void receive_new_connection_tcp(int socket_tcp, unordered_map<string, int> &online,
        vector<struct pollfd> &fds, unordered_map<string, vector<string>> &clients) {

    struct sockaddr_in tcp_client;
    socklen_t tcp_len = sizeof(tcp_client);

    /* Se accepta o conexiune noua */
    const int new_sock_fd = accept(socket_tcp, (struct sockaddr *)&tcp_client, &tcp_len);
    DIE(new_sock_fd < 0, "Error from accept.\n");

    /* Se primeste ID-ul clientului */
    char id[ID_SIZE];
    recv_all(new_sock_fd, (void *)&id, ID_SIZE);

    string client_id(id);

    /* Se verifica daca clientul se afla in baza de date cu clienti*/
    if (clients.find(client_id) != clients.end()) {
        /* Se verifica daca un client cu id-ul respectiv este deja online */
        /* Daca este, se inchide conexiunea si se afiseaza mesajul */
        auto check_online = online.find(client_id);

        if (check_online != online.end()) {
            printf("Client %s already connected.\n", client_id.c_str());
            close(new_sock_fd);
            return;
        }

    } else {
        /* Se adauga un nou client in lista de clienti */
        vector<string> topics;
        clients.insert(make_pair(client_id, topics));
    }

    /* Se insereaza in map-ul cu clientii online */
    online.insert(make_pair(client_id, new_sock_fd));

    /* Se adauga FD-ul asociat noului socket in lista pentru poll */
    add_new_poll_fd(fds, new_sock_fd);
                    
    printf("New client %s connected from %s:%d.\n", id,
        inet_ntoa(tcp_client.sin_addr), ntohs(tcp_client.sin_port));
}


/* Functia principala pentru rularea serverului */
void run_server(int socket_tcp, int socket_udp) {
    vector<struct pollfd> fds;
    bool exit_flag = false;
    int ret;

    add_new_poll_fd(fds, STDIN_FILENO);
    add_new_poll_fd(fds, socket_udp);
    add_new_poll_fd(fds, socket_tcp);

    unordered_map<string, vector<string>> clients;
    unordered_map<string, int> online;

    while (!exit_flag) {
        /* Asteptam sa primim un mesaj pe unul dintre socketii deschisi */
        ret = poll(fds.data(), fds.size(), -1);
        DIE(ret < 0, "Error caused by poll.\n");

        for (long unsigned int i = 0; i < fds.size() ; ++i) {
            if (fds[i].revents & POLLIN) {

                /* S-a primit mesaj de la standard input */
                if (fds[i].fd == STDIN_FILENO) {
                    /* Se verifica daca s-a primit mesajul de iesire a serverului */
                    exit_flag = check_exit();
                    if (exit_flag) {
                        break;
                    }

                /* S-a primit un mesaj de la un client UDP */
                } else if (fds[i].fd == socket_udp) {
                    receive_message_udp_client(socket_udp, clients, online);

                /* S-a primit o cerere de conectare a unui client TCP */
                } else if (fds[i].fd == socket_tcp) {
                    receive_new_connection_tcp(socket_tcp, online, fds, clients);

                } else {
                    /* Primim un mesaj de la un client TCP */
                    struct tcp_msg_hdr message;
                    ret = recv_all(fds[i].fd, (void *) &message, sizeof(struct tcp_msg_hdr));

                    if(ret) {
                        string id = get_client_id(online, fds[i].fd);
                        decode_client_request(id, message, clients);

                    } else {
                        /* Daca mesajul e gol (s-a primit EOF), inseamna ca conexiunea a fost
                         *inchisa de catre client, si il eliminam din lista clientilor online */
                        auto it = online.begin();
                        while (it != online.end()) {
                            if (it->second == fds[i].fd) {
                                printf("Client %s disconnected.\n", it->first.c_str());
                                it = online.erase(it);
                            } else {
                                ++it;
                            }
                        }
                        
                        /* Se inchide socket-ul si se elimina din lista pentru poll */
                        close(fds[i].fd);
                        fds.erase(fds.begin() + i);

                        i--;
                        break;
                    }

                }
            }
        }
    }

    /* Inchidem socketii aferenti clientilor conectati */
    for (unsigned long int i = 0; i < fds.size(); ++i) {
        close(fds[i].fd);
    }
}

int main(int argc, char **argv) {
    /* Dezactivare buffering */
    setvbuf(stdout, NULL, _IONBF, BUFSIZ);

    /* Verificam ca s-a primit numarul corect de argumente*/
    if (argc != 2) {
        fprintf(stderr, "Usage: ./server <PORT_DORIT>\n");
        exit(0);
    }

    uint16_t port;
    int ret = sscanf(argv[1], "%hu", &port);
    DIE(ret != 1, "Given port is invalid");

    /* Date pentru conectarea la server */
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(struct sockaddr_in));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    /* Creare socket-uri tcp si udp */
    int socket_tcp, socket_udp;
    socket_tcp = socket(AF_INET, SOCK_STREAM, 0);
    DIE(socket_tcp < 0, "Error while creating TCP socket.\n");

    socket_udp = socket(AF_INET, SOCK_DGRAM, 0);
    DIE(socket_udp < 0, "Error while creating UDP socket.\n");

    /* Adresa socketului tcp este setata ca reutilizabila */
    const int enable = 1;
    ret = setsockopt(socket_tcp, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
    DIE(ret < 0, "setsockopt(SO_REUSEADDR) failed\n");

    /* Bind pe socketurile upd si tcp */
    ret = bind(socket_tcp, (const struct sockaddr *)&server_addr, sizeof(struct sockaddr_in));
    DIE(ret < 0, "Error while trying to bind TCP socket.\n");

    ret = listen(socket_tcp, INT_MAX);
    DIE(ret < 0, "Error while trying to listen on the TCP socket.\n");

    ret = bind(socket_udp, (const struct sockaddr *)&server_addr, sizeof(struct sockaddr_in));
    DIE(ret < 0, "Error while trying to bind UDP socket.\n");

    run_server(socket_tcp, socket_udp);

    /* Se inchis socketurile */
    close(socket_tcp);
    close(socket_udp);

    return 0;
}