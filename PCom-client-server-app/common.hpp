#ifndef COMMON_HPP
#define COMMON_HPP

#include <arpa/inet.h>
#include <cctype>
#include <cerrno>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <poll.h>

using namespace std;

#define DIE(assertion, call_description)                                       \
  do {                                                                         \
    if (assertion) {                                                           \
      fprintf(stderr, "(%s, %d): ", __FILE__, __LINE__);                       \
      perror(call_description);                                                \
      exit(EXIT_FAILURE);                                                      \
    }                                                                          \
  } while (0)

#define ID_SIZE 11
#define EXIT_STR "exit"
#define MAX_BUFF_SIZE 1501

struct __attribute__((packed)) udp_msg {
  char topic[50];
  uint8_t type;
  char value[1501];
};

/* Structura header mesaje TCP pentru comunicarea client - server */
struct __attribute__((packed)) tcp_msg_hdr {
  char topic[51];
  char type[11];
  uint32_t len;
  struct in_addr ip_addr;
  uint16_t port;
};

/* Se primesc intr-un buffer len bytes veniti de pe un socket TCP */
int recv_all(int sockfd, void *buffer, size_t len);

/* Se trimit dintr-un buffer len bytes pe un socket TCP */
int send_all(int sockfd, void *buffer, size_t len);

#endif
