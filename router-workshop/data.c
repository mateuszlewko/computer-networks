#include "data.h"

#include <assert.h>
#include <stdio.h>
#include <strings.h>

struct entry read_entry() {
    struct entry e;
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


#include <limits.h>

char *int2bin2(unsigned n, char *buf)
{
    #define BITS (sizeof(n) * CHAR_BIT)

    static char static_buf[BITS + 1];
    int i;

    if (buf == NULL)
        buf = static_buf;

    for (i = BITS - 1; i >= 0; --i) {
        buf[i] = (n & 1) ? '1' : '0';
        n >>= 1;
    }

    buf[BITS] = '\0';
    return buf;

    #undef BITS
}

void set_entry_broadcast_ip(struct entry *e) {
    struct sockaddr_in server_address;
	bzero (&server_address, sizeof(server_address));

	server_address.sin_family = AF_INET;
	server_address.sin_port   = htons(54321);
    
    uint32_t m = ((1 << (32 - e->mask)) - 1);

    printf("ip A: %s\n", int2bin2(e->ip_addr, NULL));

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

void print_entry(const struct entry *e) {
    print_ip_addr((byte*)&e->ip_addr);
    printf("/%hhu distance %u ", e->mask, e->distance);

    if (e->direct) { 
        puts("connected directly");
    }
    else {
        printf("via ");
        print_ip_addr((byte*)&e->via);
        puts("");
    }
}  

void print_table(const struct table *t) {
    for (int i = 0; i < t->count; i++) {
        print_entry(&t->entries[i]);
    }
}