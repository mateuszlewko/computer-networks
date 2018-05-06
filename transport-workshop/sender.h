#pragma once

#include <stdlib.h>
#include <stdio.h>

struct sockaddr_in;

int send_request(int sockfd, const struct sockaddr_in *network_ip, int start, 
                 int length);