#pragma once

#include <stdbool.h>
#include <stdlib.h>

struct recv_result { 
    bool success;
    char ip[20];
    u_int16_t id;
    u_int16_t seq;
    struct timeval timeleft;
};

struct recv_result recv_icmp(int sockfd, struct timeval max_timeout);