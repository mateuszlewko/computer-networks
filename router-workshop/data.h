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

void add_entry(struct table *t, struct entry e);

struct table read_table();

void set_entry_broadcast_ip(struct entry *e);

void print_ip_addr(byte *ip_addr);

void print_table(const struct table *table);

void set_unreachable_to_inf(struct table* t, int64_t round);

void trim_unreachable(struct table* direct, struct table* routing, 
                      int64_t round);