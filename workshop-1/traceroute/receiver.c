#include "receiver.h"
#include "utils.h"

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>

struct recv_result recv_icmp(int sockfd, struct timeval max_timeout) {
    struct sockaddr_in sender;    
    socklen_t          sender_len = sizeof(sender);
    u_int8_t           buffer[IP_MAXPACKET];

    fd_set descriptors;
    FD_ZERO(&descriptors);
    FD_SET(sockfd, &descriptors);

    int ready = select(sockfd + 1, &descriptors, NULL, NULL, &max_timeout);
    struct recv_result result;
    result.success = true;

    if (ready < 0) {
        fprintf(stderr, "select error: %s\n", strerror(errno)); 
        exit(EXIT_FAILURE);
    }
    else if (ready == 0) {
        result.success = false;
        return result;
    }

    ssize_t packet_len = recvfrom(sockfd, buffer, IP_MAXPACKET, 0, 
                                  (struct sockaddr*)&sender, &sender_len);
        
    if (packet_len < 0) {
        fprintf(stderr, "recvfrom error: %s\n", strerror(errno)); 
        exit(EXIT_FAILURE);
    }

    const char* ntop_res = inet_ntop(AF_INET, &(sender.sin_addr), result.ip, 
                               sizeof(result.ip));

    if (ntop_res == NULL) {
        fprintf(stderr, "inet_ntop error: %s\n", strerror(errno)); 
        exit(EXIT_FAILURE);
    }
                
    struct iphdr* ip_header     = (struct iphdr*) buffer;
    ssize_t       ip_header_len = 4 * ip_header->ihl;

    ssize_t len    = packet_len - ip_header_len;
    u_int8_t* data = buffer + ip_header_len;

    result.id       = get_uint16(data, len, 2);
    result.seq      = get_uint16(data, len, 1);
    result.timeleft = max_timeout;

    return result;  
}
