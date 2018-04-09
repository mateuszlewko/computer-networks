#include "sender.h"
#include "utils.h"

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>

#include <limits.h>

char *int2bin(unsigned n, char *buf)
{
    #define BITS (sizeof(n) * CHAR_BIT)

    static char static_buf[BITS + 1];
    int i;

    if (buf == NULL)
        buf = static_buf;

    for (i = BITS - 1; i >= 0; --i) {
        buf[i] = (n & 1) ? '1' : '0';
        n >>= 1;
    }

    buf[BITS] = '\0';
    return buf;

    #undef BITS
}

int send_entry(int sockfd, const struct sockaddr_in *network_ip,
                const struct entry *e) {
    byte message[9];
    *((ip_addr_t*)message)      = e->ip_addr;
    message[4]                  = e->mask;
    *((uint32_t*)(message + 5)) = e->distance;

    // printf("ip: %s\n", int2bin(network_ip->sin_addr.s_addr, NULL));

    return sendto(sockfd, message, 9, 0, (struct sockaddr*)network_ip, 
                  sizeof(*network_ip));
}

void broadcast_table(int sockfd, struct table *direct, struct table *routing,
                     int64_t round) {
    for (int i = 0; i < direct->count; i++) {
        for (int j = 0; j < routing->count; j++) {
            int res = send_entry(sockfd, &direct->entries[i].net_broad_addr, 
                                 &routing->entries[j]);

            if (res >= 0) continue;
            
            // "The socket is connection-mode but is not connected"
            // or "No route to the network is present.""
            if (errno == ENOTCONN || errno == ENETUNREACH) {
                direct->entries[i].last_ping_round = 
                    MIN(direct->entries[i].last_ping_round, 
                        round - ROUNDS_WITHOUT_PING - 1);
                
                puts("$$error sending");
            }
            else ERROR_EXIT("sendto");
        }
    }
}