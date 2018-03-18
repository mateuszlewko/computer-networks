#include <bits/types/struct_timeval.h> 
#include <errno.h>                     
#include <netinet/in.h>                
#include <stdbool.h>                   
#include <stdio.h>                     
#include <stdlib.h>                    
#include <string.h>                    
#include <sys/socket.h>                
#include <unistd.h>                    

#include "config.h"                    
#include "receiver.h"                  
#include "sender.h"                    
#include "utils.h"    

struct args {
    char ip_str[20];
    struct sockaddr_in recipient;
};

struct args parse_args(int argc, char** argv) {
    if (argc != 2) {
        puts("ERROR: Wrong number of arguments.");
        exit(EXIT_FAILURE);
    }

    struct args ars;
    bzero(&ars, sizeof(ars));
    memcpy(ars.ip_str, argv[1], strlen(argv[1]));

    ars.recipient.sin_family = AF_INET;
    int pton_res = inet_pton(AF_INET, argv[1], 
                             &ars.recipient.sin_addr);

    if (pton_res == 0) {
        puts("Wrong host.");
        exit(EXIT_FAILURE);
    }
    else if (pton_res < 0) {
        perror("Wrong host. inet_pton error: %s\n");
        exit(EXIT_FAILURE);
    }

    return ars;
}

void trace(struct args ars) {
    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    u_int16_t pid = getpid();

    printf("pid: %d\n", pid);

    if (sockfd < 0) {
        perror("socket error: %s\n"); 
        exit(EXIT_FAILURE);
    }

    for (int ttl = 1; ttl <= MAX_TTL; ttl++) {
        struct timeval max_timeout = {WAIT_TIME_SEC, 0};
        struct timeval timeleft    = max_timeout;
        bool any_failed            = false;
        bool reached_final         = false;
        long duration_sum_ms       = 0;

        char ip_addrs[IP_LEN][PACKETS_CNT] = {0};
        int diff_ip_cnt = 0;

        for (u_int16_t i = 0; i < PACKETS_CNT; i++) {
            send_icmp(sockfd, ttl, pid, ttl * PACKETS_CNT + i, ars.recipient);
        }

        printf("%d. ", ttl);

        for (int i = 0; i < PACKETS_CNT; i++) {
            struct recv_result res = recv_icmp(sockfd, timeleft);

            if (!res.success) {
                any_failed = true;
                break;
            } // discard packet, but save left time 
            else if (res.id != pid || res.seq / PACKETS_CNT != ttl) {
                i--;
                timeleft = res.timeleft;
                continue;
            }
            else {
                // check if same ip address already received
                bool any_equal = false;
                for (int j = 0; j < diff_ip_cnt; j++) {
                    if (strcmp(ip_addrs[j], res.ip) == 0) {
                        any_equal = true;
                        break;
                    }
                }

                // if ip address is unique, save and print it
                if (!any_equal) {
                    memcpy(ip_addrs[diff_ip_cnt], res.ip, IP_LEN);
                    diff_ip_cnt++;

                    printf("%s ", res.ip);
                }

                duration_sum_ms += time_diff_ms(max_timeout, 
                                                res.timeleft);
                timeleft = res.timeleft;

                if (strcmp(res.ip, ars.ip_str) == 0) {
                    reached_final = true;
                }
            }
        }

        if (diff_ip_cnt == 0) {
            puts("*");
        }
        else if (!any_failed) {
            printf("%ldms\n", duration_sum_ms / PACKETS_CNT);
        }
        else {
            puts("???");
        }

        if (reached_final) {
            break;
        }
    }
}

int main(int argc, char** argv) {
    trace(parse_args(argc, argv));
    return EXIT_SUCCESS;
}
