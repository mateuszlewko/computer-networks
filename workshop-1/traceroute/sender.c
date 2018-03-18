#include "sender.h"
#include "utils.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <strings.h>
#include <string.h>
#include <sys/socket.h>
#include <errno.h>

void send_icmp(int sockfd, int ttl, u_int16_t id, u_int16_t seq, 
               struct sockaddr_in recipient) {
    struct icmphdr icmp_header;

    icmp_header.type             = ICMP_ECHO;
    icmp_header.code             = 0;
    icmp_header.un.echo.id       = id;
    icmp_header.un.echo.sequence = seq;
    icmp_header.checksum         = 0;
    icmp_header.checksum         = compute_icmp_checksum(
                                        (u_int16_t*)&icmp_header, 
                                        sizeof(icmp_header));

    if (setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(int)) != 0) {
        perror("setsockopt error");
        exit(EXIT_FAILURE);
    }

    ssize_t bytes_sent = sendto(sockfd, &icmp_header, sizeof(icmp_header), 0, 
                                (struct sockaddr*)&recipient, 
                                sizeof(recipient));

    if (bytes_sent < 0) {
        perror("sendto error"); 
        exit(EXIT_FAILURE);
    }                          
}