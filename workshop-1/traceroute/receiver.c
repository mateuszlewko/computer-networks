#include "receiver.h"
#include "config.h"
#include "utils.h"

#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <errno.h>
#include <time.h>

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

    clock_t t = clock();
   
    int ready = select(sockfd+1, &descriptors, NULL, NULL, &tv);
   
    t = clock() - t;
    double time_taken = ((double)t)/CLOCKS_PER_SEC; // in seconds
 
    printf("select() took %.6f seconds to execute \n", time_taken);

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
