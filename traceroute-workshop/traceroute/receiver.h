#pragma once

#include "config.h"

#include <bits/types/struct_timeval.h>
#include <stdbool.h>
#include <sys/types.h>

struct recv_result { 
    bool success;
    char ip[IP_LEN];
    u_int16_t id;
    u_int16_t seq;
    struct timeval timeleft;
};

struct recv_result recv_icmp(int sockfd, struct timeval max_timeout);