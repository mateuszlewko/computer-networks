#pragma once

#include "window.h"

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

struct sockaddr_in;

bool send_all(struct window *w, pthread_mutex_t *mutex, int sockfd, 
              const struct sockaddr_in *ip);