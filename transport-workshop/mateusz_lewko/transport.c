#include "sender.h"
#include "receiver.h"
#include "utils.h"
#include "config.h"
#include "window.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <strings.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <time.h>

int start_conn() {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0) ERROR_EXIT("socket");

	struct sockaddr_in server_address;
	bzero (&server_address, sizeof(server_address));
	server_address.sin_family      = AF_INET;
	server_address.sin_port        = htons(MY_PORT);
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	
    if (bind(sockfd, (struct sockaddr*)&server_address, 
             sizeof(server_address)) < 0) ERROR_EXIT("bind");
    
    return sockfd;
}

struct sockaddr_in create_destination(int port) {
    struct sockaddr_in dest;
    bzero (&dest, sizeof(dest));

    dest.sin_family = AF_INET;
    dest.sin_port = htons(port);
    inet_pton(AF_INET, DESTINATION_IP, &dest.sin_addr);

    return dest;
}

int main(int argc, char *argv[]) {
    char filename[1000];
    int total_length;
    int port;
    
    if (argc != 4) {
        puts("ZŁY FORMAT DANYCH.");
        exit(EXIT_FAILURE);
    }

    sscanf(argv[1], "%d", &port);
    sscanf(argv[2], "%s", filename);
    sscanf(argv[3], "%d", &total_length);
    
    FILE *file              = fopen(filename, "w");
    pthread_mutex_t mutex   = PTHREAD_MUTEX_INITIALIZER;
    int sockfd              = start_conn();
    struct sockaddr_in dest = create_destination(40001);
    struct window w         = create_window(file, total_length);

    pthread_t recv_thread = start_receiver(sockfd, &w, &mutex);
    while (send_all(&w, &mutex, sockfd, &dest)) {
        struct timespec ts;
        ts.tv_sec = SLEEP_MS / 1000;
        ts.tv_nsec = (SLEEP_MS % 1000) * 1000000;
        nanosleep(&ts, NULL);
    }

    pthread_join(recv_thread, NULL);
    fclose(file);

    return 0;
}