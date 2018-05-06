#include "receiver.h"
#include "utils.h"
#include "window.h"

#include <netinet/ip.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

struct info {
    int start;
    int length;
};

struct info receive_segment(int sockfd, char *received_data) {
    struct sockaddr_in sender;	
    socklen_t          sender_len = sizeof(sender);
    u_int8_t           buffer[IP_MAXPACKET + 1];

    ssize_t datagram_len = recvfrom(sockfd, buffer, IP_MAXPACKET, 0, 
                                    (struct sockaddr*)&sender, &sender_len);
    
    if (datagram_len < 0) ERROR_EXIT("recvfrom error");

    char sender_ip_str[20]; 
    inet_ntop(AF_INET, &(sender.sin_addr), sender_ip_str, 
              sizeof(sender_ip_str));

    struct info res;
    sscanf((char*)buffer, "DATA %d %d\n", &res.start, &res.length);
    memcpy(received_data, strchr((char*)buffer, '\n') + 1, res.length);
    
    // printf("received; start: %d, len : %d\n", res.start, res.length);

    return res;
}

struct receiver_args {
    int sockfd;
    struct window *w;
    pthread_mutex_t *mutex;
};

bool check(const struct window *w, pthread_mutex_t *mutex) {
    pthread_mutex_lock(mutex);
    bool res = !all_done(w);
    pthread_mutex_unlock(mutex);

    return res;
}

void *receiver(void *args) {
    struct receiver_args a = *(struct receiver_args*)args;
    free(args);

    while (check(a.w, a.mutex)) {
        char data[SEGMENT_LEN];
        struct info i = receive_segment(a.sockfd, data);

        if (i.length > 0) {
            int segment = i.start / SEGMENT_LEN;
            set_received(a.w, segment, data);
        }
    }

    return NULL;
}

pthread_t start_receiver(int sockfd, struct window *w, pthread_mutex_t *mutex) {
    pthread_t thread_id;
    struct receiver_args *args = malloc(sizeof(struct receiver_args));
    args->sockfd = sockfd;
    args->w      = w;
    args->mutex  = mutex;

    pthread_create(&thread_id, NULL, receiver, (void*)args);
    return thread_id;
}