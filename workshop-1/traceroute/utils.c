#include "utils.h"

#include <assert.h>
#include <bits/types/struct_timeval.h>

u_int16_t compute_icmp_checksum (const void *buff, int length) {
	u_int32_t sum;
	const u_int16_t *ptr = buff;

	assert (length % 2 == 0);

	for (sum = 0; length > 0; length -= 2)
		sum += *ptr++;

	sum = (sum >> 16) + (sum & 0xffff);
	return (u_int16_t)(~(sum + (sum >> 16)));
}

u_int16_t get_uint16(const u_int8_t *buff, ssize_t len, ssize_t end_pos) {
	ssize_t len2B = len / 2;
	return (((u_int16_t*)buff)[len2B - end_pos]);
}

long time_diff_ms(struct timeval before, struct timeval after) {
	return (before.tv_sec - after.tv_sec) * 1000L 
	       + (before.tv_usec - after.tv_usec) / 1000L;
}