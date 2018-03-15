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
#include <unistd.h>

char* parse_args(int argc, char** argv) {
    return argv[1];
}

void trace(char* ip_addr) {
    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    int pid = getpid();
    printf("pid: %d\n", pid);

    if (sockfd < 0) {
        fprintf(stderr, "socket error: %s\n", strerror(errno)); 
        exit(EXIT_FAILURE);
    }

    for (int ttl = 1; ttl <= 30; ttl++) {
        send_icmp(sockfd, ip_addr, ttl, pid, ttl);
        recv_icmp(sockfd);
    }
}

int main(int argc, char** argv) {
    trace(parse_args(argc ,argv));
    return EXIT_SUCCESS;
}
