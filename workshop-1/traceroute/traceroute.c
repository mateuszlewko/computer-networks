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
 
void fail_bad_host() {
    puts("ERROR: Bad host format.");
    exit(EXIT_FAILURE);
}

char* parse_args(int argc, char** argv) {
    if (argc != 2) {
        puts("ERROR: Wrong number of arguments.");
        exit(EXIT_FAILURE);
    }

    int cnt  = 0;
    int curr = 0;
    bool was_zero = false;
    bool was_any = false;

    for (size_t i = 0; i < strlen(argv[1]); i++) {
        if (argv[1][i] == '.' && (!was_any || curr > 255 || curr < 0)) {
            fail_bad_host();
        }
        else if (argv[1][i] == '.') {
            was_zero = false;
            was_any = false;
            cnt++;
            curr = 0;
            continue;
        }

        if (argv[1][i] == '0' && curr == 0 && was_zero) fail_bad_host();
        if (argv[1][i] == '0') was_zero = true;

        was_any = true;
        curr *= 10;
        curr += argv[1][i] - '0';
    }

    if (!was_any || curr > 255 || curr < 0 || cnt != 3) fail_bad_host();

    return argv[1];
}

void trace(char* target_ip) {
    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    int pid = getpid();

    if (sockfd < 0) {
        fprintf(stderr, "socket error: %s\n", strerror(errno)); 
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

        for (int i = 0; i < PACKETS_CNT; i++) {
            send_icmp(sockfd, target_ip, ttl, pid, ttl * MAX_TTL + i);
        }

        printf("%d. ", ttl);

        for (int i = 0; i < PACKETS_CNT; i++) {
            struct recv_result res = recv_icmp(sockfd, timeleft);

            if (!res.success) {
                any_failed = true;
            } /* discard packet, but save left time */
            else if (res.id != pid || res.seq / MAX_TTL != ttl) {
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

                if (strcmp(res.ip, target_ip) == 0) {
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
    trace(parse_args(argc ,argv));
    return EXIT_SUCCESS;
}
