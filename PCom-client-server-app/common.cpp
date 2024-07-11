#include "common.hpp"

int send_all(int sockfd, void *buffer, size_t len) {
  size_t bytes_sent = 0;
  size_t bytes_remaining = len;
  int ret;
  char *buff = (char *) buffer;

  while(bytes_remaining) {
    ret = send(sockfd, buff + bytes_sent, bytes_remaining, 0);
    DIE(ret < 0, "Error from send.\n");

    bytes_sent += ret;
    bytes_remaining -= ret;
    if(!ret) break;
  }

  return bytes_sent;
}

int recv_all(int sockfd, void *buffer, size_t len) {
  size_t bytes_received = 0;
  size_t bytes_remaining = len;
  int ret;
  char *buff = (char *) buffer;

  while(bytes_remaining) {
    ret = recv(sockfd, buff + bytes_received, bytes_remaining, 0);
    DIE(ret < 0, "Error from recv.\n");

    bytes_received += ret;
    bytes_remaining -= ret;
    if(!ret) break;
  }
  
  return bytes_received;
}