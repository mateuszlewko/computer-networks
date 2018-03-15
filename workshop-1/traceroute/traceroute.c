#include "receiver.h"
#include "sender.h"

#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <errno.h>

int main() {
    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0) {
        fprintf(stderr, "socket error: %s\n", strerror(errno)); 
        return EXIT_FAILURE;
    }

    char* addr = "8.8.8.8";

    for (int ttl = 1; ttl <= 30; ttl++) {
        send_icmp(sockfd, addr, ttl);
        recv_icmp(sockfd);
    }

    return EXIT_SUCCESS;
}
