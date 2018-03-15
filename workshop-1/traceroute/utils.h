#pragma once

#include <sys/types.h>
struct timeval;

u_int16_t compute_icmp_checksum (const void *buff, int length);

u_int16_t get_uint16(const u_int8_t *buff, ssize_t len, ssize_t end_pos);

long time_diff_ms(struct timeval before, struct timeval after);