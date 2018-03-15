#pragma once

#include <stdlib.h>
#include <stdio.h>

void send_icmp(int sockfd, char *ip_addr, int ttl, u_int16_t id, u_int16_t seq);