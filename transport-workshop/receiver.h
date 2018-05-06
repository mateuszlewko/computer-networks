#pragma once

#include "window.h"
#include <pthread.h>

pthread_t start_receiver(int sockfd, struct window *w, pthread_mutex_t *mutex);