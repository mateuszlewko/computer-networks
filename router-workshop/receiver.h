#pragma once 

#include "config.h"
#include "data.h"
#include "utils.h"

struct recv_result { 
    bool           success;
    bool           tle;
    struct timeval timeleft;
    ip_addr_t      ip_addr;

    uint32_t       entry_distance;
    ip_addr_t      entry_ip;
    byte           entry_mask;
};

struct recv_result receive_entry(int sockfd, struct timeval max_timeout);

void start_receive_round(struct table *direct, struct table *routing, 
                         int sockfd, int64_t round);