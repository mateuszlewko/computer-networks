#include "sender.h"

#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <errno.h>

void send_icmp(int sockfd, char *ip_addr, int ttl) {
    struct icmphdr icmp_header;

    icmp_header.type             = ICMP_ECHO;
    icmp_header.code             = 0;
    icmp_header.un.echo.id       = 1; // TODO
    icmp_header.un.echo.sequence = 1; // TODO
    icmp_header.checksum         = 0;
    icmp_header.checksum         = compute_icmp_checksum(
                                        (u_int16_t*)&icmp_header, 
                                        sizeof(icmp_header));

    struct sockaddr_in recipient;
    bzero(&recipient, sizeof(recipient));
    recipient.sin_family = AF_INET;
    inet_pton(AF_INET, ip_addr, &recipient.sin_addr);

    setsockopt (sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(int));

    ssize_t bytes_sent = sendto(sockfd, &icmp_header, sizeof(icmp_header), 0,
                                (struct sockaddr*)&recipient, 
                                sizeof(recipient));
}