#pragma once

#include "config.h"
#include "data.h"

int send_entry(int sockfd, const struct sockaddr_in *network_ip,
                const struct entry *e);

void broadcast_table(int sockfd, struct table *direct, struct table *routing,
                     int64_t round);