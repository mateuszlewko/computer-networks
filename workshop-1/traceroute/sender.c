#include "sender.h"
#include "utils.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <strings.h>
#include <string.h>
#include <sys/socket.h>
#include <errno.h>

void send_icmp(int sockfd, char *ip_addr, int ttl, u_int16_t id, u_int16_t seq) {
    struct icmphdr icmp_header;

    icmp_header.type             = ICMP_ECHO;
    icmp_header.code             = 0;
    icmp_header.un.echo.id       = id;
    icmp_header.un.echo.sequence = seq;
    icmp_header.checksum         = 0;
    icmp_header.checksum         = compute_icmp_checksum(
                                        (u_int16_t*)&icmp_header, 
                                        sizeof(icmp_header));

    struct sockaddr_in recipient;
    bzero(&recipient, sizeof(recipient));
    recipient.sin_family = AF_INET;
    int pton_res = inet_pton(AF_INET, ip_addr, 
                             &recipient.sin_addr);

    if (pton_res == 0) {
        puts("Host does not contain a character string representing a valid \
              network address in the specified address family.");
        exit(EXIT_FAILURE);
    }
    else if (pton_res < 0) {
        fprintf(stderr, "inet_pton error: %s\n", strerror(errno)); 
        exit(EXIT_FAILURE);
    }

    setsockopt (sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(int));

    ssize_t bytes_sent = sendto(sockfd, &icmp_header, sizeof(icmp_header), 0, 
                                (struct sockaddr*)&recipient, 
                                sizeof(recipient));

    if (bytes_sent < 0) {
        fprintf(stderr, "sendto error: %s\n", strerror(errno)); 
        exit(EXIT_FAILURE);
    }                          
}