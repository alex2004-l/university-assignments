#include <iostream>
#include <cstdlib>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <cstring>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include <string>
#include "helpers.hpp"
#include "request.hpp"
#include "nlohmann/json.hpp"

char *compute_get_request(const char *host, const char *url, char *query_params,
                            const char *cookies, const char *token) {

    char *message = (char *) calloc(BUFLEN, sizeof(char));
    char *line = (char *) calloc(LINELEN, sizeof(char));

    // write the method name, URL, request params (if any) and protocol type
    if (query_params != NULL) {
        sprintf(line, "GET %s?%s HTTP/1.1", url, query_params);
    } else {
        sprintf(line, "GET %s HTTP/1.1", url);
    }

    compute_message(message, line);

    // Add the host

    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    // Add cookies and token fields

    if (cookies != NULL) {
       sprintf(line, "Cookie: %s", cookies);

       compute_message(message, line);
    }

    if (token != NULL) {
        sprintf(line, "Authorization: Bearer %s", token);
        compute_message(message, line);
    }

    // add final new line
    compute_message(message, "");
    free(line);
    return message;
}

char *compute_post_request(const char *host, const char *url, const char* content_type,
                                const char* data, const char *token) {
    char *message = (char *) calloc(BUFLEN, sizeof(char));
    char *line = (char *) calloc(LINELEN, sizeof(char));

    // write the method name, URL and protocol type
    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);
    
    // Add the host

    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    // Add content-type and content-length
    sprintf(line, "Content-Type: %s", content_type);
    compute_message(message, line);

    sprintf(line, "Content-Length: %lu", strlen(data));
    compute_message(message, line);

    // Add authorization token

    if (token) {
        sprintf(line, "Authorization: Bearer %s", token);
        compute_message(message, line);
    }

    compute_message(message, "");

    // add the actual payload data
    compute_message(message, data);
    
    free(line);

    return message;
}


char *compute_delete_request(const char *host, const char* url,
                            const char *cookies, const char *token) {
    
    char *message = (char *) calloc(BUFLEN, sizeof(char));
    char *line = (char *) calloc(LINELEN, sizeof(char));

    sprintf(line, "DELETE %s HTTP/1.1", url);
    compute_message(message, line);

    /* Add hosts */
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    /* Add cookies and token */
    if (cookies != NULL) {
       sprintf(line, "Cookie: %s", cookies);
       compute_message(message, line);
    }

    if (token) {
        sprintf(line, "Authorization: Bearer %s", token);
        compute_message(message, line);
    }

    compute_message(message, "");

    free(line);
    return message;
}

char *send_server_request(const char *path, REQUESTS req, string &cookies, string &token, const char *data) {
    int sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
    char *message;

    const char *token_char = NULL;
    const char *cookies_char = NULL;
    if (!token.empty()) token_char = token.c_str();
    if (!cookies.empty()) cookies_char = cookies.c_str();

    /* Compute message*/
    switch(req) {
        case POST:
            message = compute_post_request(HOST, path, CONTENT_TYPE, data, token_char);
            break;
        case GET:
            message = compute_get_request(HOST, path, NULL, cookies_char, token_char);
            break;
        case DELETE:
            message = compute_delete_request(HOST, path, cookies_char, token_char);
            break;
    }
    send_to_server(sockfd, message);
    free(message);

    char *response = receive_from_server(sockfd);
    close_connection(sockfd);

    return response;

}