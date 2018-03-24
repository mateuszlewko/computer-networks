#pragma once

#include <stdlib.h>
#include <stdio.h>

struct sockaddr_in;

void send_icmp(int sockfd, int ttl, u_int16_t id, u_int16_t seq, 
               struct sockaddr_in recipient);