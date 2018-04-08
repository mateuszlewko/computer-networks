#pragma once

#include "config.h"
#include "utils.h"

#include <stdbool.h>
#include <stdint.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

typedef uint32_t ip_addr_t;

struct entry {
    ip_addr_t ip_addr;
    byte      mask;
    bool      direct;
    ip_addr_t via;
    uint32_t  distance;
    int64_t   last_ping_round;

    struct sockaddr_in net_broad_addr;
};

struct table {
    int          count;
    struct entry entries[MAX_TABLE_SIZE];
};

struct table read_table();

void print_table(const struct table *table);
