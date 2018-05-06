#pragma once

#include <sys/types.h>
struct timeval;

u_int16_t compute_icmp_checksum (const void *buff, int length);

long time_diff_ms(struct timeval before, struct timeval after);