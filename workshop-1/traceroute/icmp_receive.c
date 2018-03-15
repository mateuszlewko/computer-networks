#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <errno.h>

#include "utils.h"

void print_as_bytes (unsigned char *buff, ssize_t length) {
    for (ssize_t i = 0; i < length; i++, buff++) {
        printf ("%.2x ", *buff);    
    }
}

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

int recv_icmp(int sockfd) {
    struct sockaddr_in sender;    
    socklen_t          sender_len = sizeof(sender);
    u_int8_t           buffer[IP_MAXPACKET];

    fd_set descriptors;
    FD_ZERO (&descriptors);
    FD_SET (sockfd, &descriptors);
    struct timeval tv; 
    tv.tv_sec = WAIT_TIME_SEC; 
    tv.tv_usec = 0;

    int ready = select(sockfd+1, &descriptors, NULL, NULL, &tv);

    if (ready < 0) {
        fprintf(stderr, "select error: %s\n", strerror(errno)); 
        exit(EXIT_FAILURE);
    }
    else if (ready == 0) return 0;

    ssize_t packet_len = recvfrom(sockfd, buffer, IP_MAXPACKET, 0, 
                                  (struct sockaddr*)&sender, &sender_len);
        
    if (packet_len < 0) {
        fprintf(stderr, "recvfrom error: %s\n", strerror(errno)); 
        exit(EXIT_FAILURE);
    }

    char sender_ip_str[20]; 
    inet_ntop(AF_INET, &(sender.sin_addr), sender_ip_str, 
              sizeof(sender_ip_str));
                
    printf ("Received IP packet with ICMP content from: %s\n", sender_ip_str);

    struct iphdr* ip_header     = (struct iphdr*) buffer;
    ssize_t       ip_header_len = 4 * ip_header->ihl;

    printf ("IP header: "); 
    print_as_bytes (buffer, ip_header_len);
    printf("\n");

    printf ("IP data:   ");
    print_as_bytes (buffer + ip_header_len, packet_len - ip_header_len);
    printf("\n\n");
}

int main()
{
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
