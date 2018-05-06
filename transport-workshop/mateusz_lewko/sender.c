#include "sender.h"
#include "config.h"
#include "utils.h"
#include "window.h"

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

bool send_all(struct window *w, pthread_mutex_t *mutex, int sockfd, 
              const struct sockaddr_in *ip) {
    pthread_mutex_lock(mutex);

    if (all_done(w)) {
        pthread_mutex_unlock(mutex);
        return false;
    }

    int segs_left = w->total_segments - w->curr_segment_saved - 1;

    for (int i = 1; i <= MIN(SEGMENTS_CNT, segs_left); i++) {
        int segment = i + w->curr_segment_saved;
        if (w->received[segment % SEGMENTS_CNT])
            continue;

        int offset = SEGMENT_LEN * segment;
        send_request(sockfd, ip, offset, segment_len(w, segment));
    }

    pthread_mutex_unlock(mutex);
    return true;
}