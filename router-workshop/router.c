#include "data.h"
#include "sender.h"
#include "receiver.h"
#include "data.h"

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

int start_server() {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0) ERROR_EXIT("receiver socket");

	struct sockaddr_in server_address;
	bzero (&server_address, sizeof(server_address));
	server_address.sin_family      = AF_INET;
	server_address.sin_port        = htons(54321);
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	
    if (bind(sockfd, (struct sockaddr*)&server_address, 
             sizeof(server_address)) < 0) ERROR_EXIT("bind");
    
    return sockfd;
}

int main() {
    struct table direct  = read_table();
    struct table routing = direct;

    for (int i = 0; i < direct.count; i++) {
        set_entry_broadcast_ip(&direct.entries[i]);
    }

    print_table(&direct);

    int sender_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sender_sockfd < 0) ERROR_EXIT("sender socket");

    int receiver_sockfd = start_server();
    
    int64_t round = 0;
    while (++round) {
        broadcast_table(sender_sockfd, &direct, &routing, round);
        start_receive_round(&direct, &routing, receiver_sockfd, round);

        print_table(&routing);
        puts("");
    }
}