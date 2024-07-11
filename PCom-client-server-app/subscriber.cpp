#include <netdb.h>

#include "common.hpp"

/* FUnctia principala de rulare a clientului */
void run_client(int sockfd, char* id) {

    send_all(sockfd, id, ID_SIZE);
    char buf[MAX_BUFF_SIZE];

    struct pollfd poll_fds[2];
    int num_sockets = 2;
    int ret;
    bool exit_flag = false;

    poll_fds[0].fd = STDIN_FILENO;
    poll_fds[0].events = POLLIN;

    poll_fds[1].fd = sockfd;
    poll_fds[1].events = POLLIN;

    while (!exit_flag) {
        ret = poll(poll_fds, num_sockets, -1);
        DIE(ret < 0, "poll");

        for (int i = 0; i < num_sockets; i++) {
            if (poll_fds[i].revents & POLLIN) {
                if (i == 0) {
                    char input[128];
                    size_t input_len;

                    /* Se citeste comanda de la stdin */
                    fgets(input, sizeof(input), stdin);
                    input_len = strlen(input);

                    while (isspace(input[input_len - 1])) {
                        input[input_len - 1] = '\0';
                        input_len --;
                    }

                    /* Se verifica daca este stringul de iesire */
                    if (!strcmp(input, EXIT_STR)) {
                        exit_flag = true;
                        break;
                    }

                    /* Se imparte in comanda si topic sirul primit */
                    char *command, *topic;
                    command = strtok(input, " ");

                    if (command == NULL) {
                        continue;
                    }

                    topic = strtok(NULL, " ");

                    if (topic == NULL) {
                        continue;
                    }

                    /* Daca comanda este valida, se trimite o cerere catre server */
                    struct tcp_msg_hdr request;
                    memset(&request, 0, sizeof(struct tcp_msg_hdr));
                    if (!strcmp(command, "subscribe")) {
                        strcpy(request.type, "SUB");
                        strcpy(request.topic, topic);
                        send_all(sockfd, (void *) &request, sizeof(struct tcp_msg_hdr));
                        printf("Subscribed to topic %s\n", request.topic);

                    } else if (!strcmp(command, "unsubscribe")) {
                        strcpy(request.type, "UNSUB");
                        strcpy(request.topic, topic);
                        send_all(sockfd, (void *) &request, sizeof(struct tcp_msg_hdr));
                        printf("Unsubscribed from topic %s\n", request.topic);

                    }
                } else {
                    /* Se primesc un header si un pachet de la server */
                    struct tcp_msg_hdr recv_packet;
                    int ret = recv_all(sockfd, &recv_packet, sizeof(recv_packet));

                    if (!ret) {
                        exit_flag = true;
                        break;
                    }

                    ret = recv_all(sockfd, &buf, recv_packet.len);
                    
                    if (!ret) {
                        exit_flag = true;
                        break;
                    }

                    printf("%s:%d - %s - %s - %s\n", inet_ntoa(recv_packet.ip_addr), recv_packet.port,
                        recv_packet.topic, recv_packet.type, buf);

                }
            }
        }
    }

}


int main(int argc, char **argv) {

    /* Dezactivare buffering s*/
    setvbuf(stdout, NULL, _IONBF, BUFSIZ);

    /* Verificare numar corect argumente */
    if (argc != 4) {
        fprintf(stderr, "Usage: ./subscriber <ID_CLIENT> <IP_SERVER> <PORT_SERVER>\n");
        exit(0);
    }

    uint16_t port;
    int ret = sscanf(argv[3], "%hu", &port);
    DIE(ret != 1, "Given port is invalid");

    /* Se creeaza socket-ul TCP */
    const int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    DIE(sockfd < 0, "Error while creating socket.\n");

    /* Se dezactiveaza algoritmul lui Nagle */
    int enable = 1;
    if (setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, &enable, sizeof(enable)) < 0) {
        perror("Failed to set TCP_NODELAY option");
    }

    /* Se conecteaza la server */
    struct sockaddr_in serv_addr;
    socklen_t socket_len = sizeof(struct sockaddr_in);

    memset(&serv_addr, 0, socket_len);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    int rc = inet_pton(AF_INET, argv[2], &serv_addr.sin_addr.s_addr);
    DIE(rc <= 0, "Error caused by inet_pton.\n");

    rc = connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    DIE(rc < 0, "Couldn't connect to server.\n");

    run_client(sockfd, argv[1]);

    /* Se inchide socket-ul */
    close(sockfd);

    return 0;
}