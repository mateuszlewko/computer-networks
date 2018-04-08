#include "receiver.h"
#include "utils.h"
#include "data.h"

#include <netinet/ip.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

struct recv_result receive_entry(int sockfd, struct timeval max_timeout) {
    struct sockaddr_in sender;	
    socklen_t          sender_len = sizeof(sender);
    u_int8_t           buffer[IP_MAXPACKET + 1];

    fd_set descriptors;
    FD_ZERO(&descriptors);
    FD_SET(sockfd, &descriptors);

    int ready = select(sockfd + 1, &descriptors, NULL, NULL, &max_timeout);
    struct recv_result result = { .success = false, .timeleft = max_timeout, 
                                  .ip_addr = 0, .tle = false };

    if (ready < 0) ERROR_EXIT("select error");
    else if (ready == 0) {
        result.tle = true;
        return result;
    }

    ssize_t datagram_len = recvfrom(sockfd, buffer, IP_MAXPACKET, 0, 
                                    (struct sockaddr*)&sender, &sender_len);
    
    if (datagram_len < 0) ERROR_EXIT("recvfrom error");
    if (datagram_len != 9) return result;

    result.success = true;

    char sender_ip_str[20]; 
    inet_ntop(AF_INET, &(sender.sin_addr), sender_ip_str, 
              sizeof(sender_ip_str));

    byte *ip = (byte*)&result.ip_addr;
    sscanf(sender_ip_str, "%hhu.%hhu.%hhu.%hhu", &ip[0], &ip[1], &ip[2]
                                               , &ip[3]);

    // first four bytes are network ip address
    result.entry_ip = *((ip_addr_t*)buffer);
    // fifth byte is network mask
    result.entry_mask = (byte)buffer[4];
    // bytes 5 - 8 are distance
    result.entry_distance = *((uint32_t*)(buffer + 5));

    return result;
}

bool is_from_network(ip_addr_t network_ip, byte mask, ip_addr_t ip) {
    ip_addr_t check_mask = ((0xffffffff) >> (32 - mask)) << (32 - mask);
    return (network_ip ^ (ip & check_mask)) == 0;
}

void start_receive_round(struct table *direct, struct table *routing, 
                         int sockfd, int64_t round) {
    struct timeval duration;
    duration.tv_sec = ROUND_DURATION_SEC;
    duration.tv_usec = 0;

    while (duration.tv_sec + duration.tv_usec > 0) {
        // printf("bef | sec: %ld, usec: %ld\n", duration.tv_sec, duration.tv_usec);
        
        struct recv_result r = receive_entry(sockfd, duration);
        duration = r.timeleft;

        if (r.tle) return;
        if (!r.success)
            continue;

        printf("received from: ");
        print_ip_addr((byte*)&r.ip_addr);
        printf(" about: ");
        print_ip_addr((byte*)&r.entry_ip);
        printf(" distance: %d\n", r.entry_distance);
            
        // printf("af | sec: %ld, usec: %ld\n", duration.tv_sec, duration.tv_usec);

        for (int i = 0; i < direct->count; i++) {
            struct entry *e = &direct->entries[i];
            if (is_from_network(e->ip_addr, e->mask, r.ip_addr)) {
                printf("from network: ");
                print_ip_addr((byte*)&e->ip_addr);
                puts("");

                e->last_ping_round = round;
                bool found = true;

                for (int j = 0; j < routing->count; j++) {
                    struct entry *re = &routing->entries[j];

                    if (re->ip_addr == r.entry_ip 
                        && re->mask == r.entry_mask) {

                        if ((uint64_t)e->distance + r.entry_distance 
                                < re->distance) {
                            re->distance = e->distance + r.entry_distance;
                            re->via = r.ip_addr;
                        }
                        
                        found = true;
                        break;
                    }
                }

                printf("found: %d\n", found);

                if (!found) {
                    struct entry new_entry = { 
                        .distance = e->distance + r.entry_distance,
                        .via      = r.ip_addr,
                        .ip_addr  = r.entry_ip,
                        .mask     = r.entry_mask,
                        .direct   = false
                    };

                    add_entry(routing, new_entry);
                }

                break;
            }
        }        
    }
}