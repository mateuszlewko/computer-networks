#pragma once
#include <stdlib.h>

u_int16_t compute_icmp_checksum (const void *buff, int length);

void print_as_bytes (unsigned char* buff, ssize_t length);

u_int16_t get_uint16(const char *buff, ssize_t len, ssize_t end_pos);