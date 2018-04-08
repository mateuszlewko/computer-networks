#include "data.h"

#include <assert.h>
#include <stdio.h>

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

void print_entry(struct entry *e) {
    print_ip_addr((byte*)&e->ip_addr);
    printf("/%hhu distance %ud ", e->mask, e->distance);

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