#ifndef REQUESTS_HPP
#define REQUESTS_HPP

#include <string>
using namespace std;

#define HOST "34.246.184.49"
#define PORT 8080
#define CONTENT_TYPE "application/json"

enum REQUESTS {
    POST,
    GET,
    DELETE
};

// computes and returns a GET request string (query_params
// and cookies can be set to NULL if not needed)
char *compute_get_request(const char *host, const char *url, char *query_params,
							const char *cookies, const char *token);

// computes and returns a POST request string (cookies can be NULL if not needed)
char *compute_post_request(const char *host, const char *url, const char* content_type,
							const char *data, const char *token);

char *compute_delete_request(const char *host, const char* url, const char *cookies,
							const char *token);

char *send_server_request(const char *path, REQUESTS req, string &cookies,
							string &token, const char *data);

#endif
