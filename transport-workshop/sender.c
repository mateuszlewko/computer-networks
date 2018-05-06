#include "sender.h"
#include "config.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <strings.h>
#include <string.h>
#include <sys/socket.h>
#include <errno.h>
#include <stdbool.h>
#include <pthread.h>

int send_request(int sockfd, const struct sockaddr_in *ip, int start, 
                 int length) {
    char message[1000];
    sprintf(message, "GET %d %d\n", start, length);

    return sendto(sockfd, message, strlen(message), 0, (struct sockaddr*)ip, 
                  sizeof(*ip));
}

void send_all(bool *received, pthread_mutex_t *mutex, int sockfd, 
              const struct sockaddr_in *ip, int offset, int count, 
              int total_length) {
    pthread_mutex_lock(mutex);

    for (int i = 0; i < count; i++) {
        if (received[i])
            continue;

        int left_len = total_length - (offset + i) * SEGMENT_LEN;
        send_request(sockfd, ip, offset + i, min(SEGMENT_LEN, left_len));
    }

    pthread_mutex_unlock(mutex);
}