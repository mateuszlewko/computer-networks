#include "data.h"
#include "sender.h"
#include "receiver.h"
#include "data.h"
#include "utils.h"

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <strings.h>

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

    // print_table(&direct, 0);

    int sender_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sender_sockfd < 0) ERROR_EXIT("sender socket");

    int broadcastEnable=1;
    setsockopt(sender_sockfd, SOL_SOCKET, SO_BROADCAST, 
                       &broadcastEnable, sizeof(broadcastEnable));


    int receiver_sockfd = start_server();
    
    int64_t round = 0;
    while (++round > 0) {
        broadcast_table(sender_sockfd, &direct, &routing, round);
        start_receive_round(&direct, &routing, receiver_sockfd, round);
        set_unreachable_to_inf(&routing, round);
        // set_unreachable_to_inf(&direct, round);
        trim_unreachable(&direct, &routing, round);
        add_from_direct(&direct, &routing, round);

        print_table(&routing, round);
        puts("");
    }
}