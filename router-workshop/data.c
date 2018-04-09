#include "data.h"

#include <assert.h>
#include <stdio.h>
#include <strings.h>

struct entry read_entry() {
    struct entry e;
    e.last_ping_round = 0;
    e.via = 0;
    byte *ip = (byte*)&e.ip_addr;

    scanf("%hhu.%hhu.%hhu.%hhu/%hhu distance %ud", &ip[0], &ip[1], &ip[2]
                                                 , &ip[3], &e.mask
                                                 , &e.distance);
    e.direct = true;

    return e;
}

void add_entry(struct table *t, struct entry e) {
    assert(t->count < MAX_TABLE_SIZE);
    t->entries[t->count++] = e;
}

void set_entry_broadcast_ip(struct entry *e) {
    struct sockaddr_in server_address;
	bzero (&server_address, sizeof(server_address));

	server_address.sin_family = AF_INET;
	server_address.sin_port   = htons(54321);
    
    uint32_t m = ((1 << (32 - e->mask)) - 1);

    // printf("ip A: %s\n", int2bin2(e->ip_addr, NULL));

	server_address.sin_addr.s_addr = e->ip_addr | htonl(m);

    e->net_broad_addr = server_address;
}

struct table read_table() {
    int n;
    scanf("%d", &n);

    struct table t;
    for (int i = 0; i < n; i++) {
        add_entry(&t, read_entry());
    }

    return t;
}

void print_ip_addr(byte *ip_addr) {
    printf("%hhu.%hhu.%hhu.%hhu", ip_addr[0], ip_addr[1], ip_addr[2]
                                , ip_addr[3]);
}

void print_entry(const struct entry *e, int64_t round) {
    ip_addr_t ip = e->ip_addr & ((0xffffffff) >> (32 - e->mask));

    print_ip_addr((byte*)&ip);
    bool reachable = e->distance < INF_DIST 
                     && e->last_ping_round > round - ROUNDS_WITHOUT_PING;

    if (reachable)
        printf("/%hhu distance %u ", e->mask, e->distance);
    else printf("/%hhu unreachable ", e->mask);

    if (e->direct || !reachable) { 
        puts("connected directly");
    }
    else {
        printf("via ");
        print_ip_addr((byte*)&e->via);
        puts("");
    }
}  

void print_table(const struct table *t, int64_t round) {
    for (int i = 0; i < t->count; i++) {
        print_entry(&t->entries[i], round);
    }
}

void set_unreachable_to_inf(struct table* t, int64_t round) {
    for (int i = 0; i < t->count; i++) {
        struct entry *e = &t->entries[i];

        if (e->last_ping_round < round - ROUNDS_WITHOUT_PING
            || e->distance >= INF_DIST) {
            e->distance = (uint32_t)((1LL<<32) - 1);
        }   
    }
}

void trim_unreachable(struct table* direct, struct table* routing, 
                      int64_t round) {
    for (int i = 0; i < direct->count; i++) {
        struct entry *e = &direct->entries[i];

        if (e->last_ping_round < round - ROUNDS_SEND_UNREACHABLE) {
            for (int j = 0; j < routing->count; j++) {
                if (routing->entries[j].via == e->ip_addr 
                    && !routing->entries[j].direct) {
                    SWAP(routing->entries[j], routing->entries[routing->count]);
                    j--;
                    routing->count--;
                }
            }
        }
    }

    for (int i = 0; i < routing->count; i++) {
        struct entry *re = &routing->entries[i];

        if (!re->direct 
            && re->last_ping_round < round - ROUNDS_SEND_UNREACHABLE) {
            SWAP(routing->entries[i], routing->entries[routing->count]);
            i--;
            routing->count--;
        }
    }
}

bool is_from_network(ip_addr_t network_ip, byte mask, ip_addr_t ip) {
    // puts("--- testing: is from network ---");
    ip_addr_t check_mask = ((0xffffffff) >> (32 - mask));
    // printf("check_mask 1) : "); print_ip_addr((byte*)&check_mask);
    check_mask = check_mask << (32 - mask);
    check_mask = htonl(check_mask);
    // printf("check_mask 2) : "); print_ip_addr((byte*)&check_mask);
    
    
    // printf("\nnetwork_ip & check_mask: "); 
    ip_addr_t a = network_ip & check_mask;
    // print_ip_addr((byte*)&a);

    // printf("\nip & check_mask: "); 
    ip_addr_t b = ip & check_mask;
    // print_ip_addr((byte*)&b);

    // puts("\n");

    return ((network_ip & check_mask) ^ (ip & check_mask)) == 0;
}

void add_from_direct(struct table* direct, struct table* routing, 
                     int64_t round) {
    for (int i = 0; i < direct->count; i++) {
        struct entry *e = &direct->entries[i];

        // if (e->last_ping_round > round - ROUNDS_WITHOUT_PING
        //     && e->distance < INF_DIST) {
        bool found = false;

        for (int j = 0; j < routing->count; j++) {
            struct entry *re = &routing->entries[j];
            if (re->mask == e->mask 
                && is_from_network(re->ip_addr, e->mask, e->ip_addr)) {
                if (e->distance < re->distance
                    && e->last_ping_round > round - ROUNDS_WITHOUT_PING) {
                    re->distance = e->distance;
                    re->direct = true;
                    re->via = 0;
                    re->last_ping_round = e->last_ping_round;
                    re->ip_addr = e->ip_addr;
                }
                
                found = true;
                break;
            }
        }

        if (!found) {
            add_entry(routing, *e);
        }
        // }
    }
}