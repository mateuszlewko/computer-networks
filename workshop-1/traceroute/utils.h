#pragma once
#include <stdlib.h>

u_int16_t compute_icmp_checksum (const void *buff, int length);

void print_as_bytes (unsigned char* buff, ssize_t length);