#pragma once

#include <stdlib.h>
#include <stdio.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <strings.h>
#include <string.h>
#include <sys/socket.h>
#include <errno.h>

void send_icmp(int sockfd, int ttl, u_int16_t id, u_int16_t seq, 
               struct sockaddr_in recipient);