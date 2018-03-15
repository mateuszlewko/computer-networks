#include "utils.h"

#include <assert.h>
#include <stdio.h>

u_int16_t compute_icmp_checksum (const void *buff, int length) {
	u_int32_t sum;
	const u_int16_t *ptr = buff;

	assert (length % 2 == 0);

	for (sum = 0; length > 0; length -= 2)
		sum += *ptr++;

	sum = (sum >> 16) + (sum & 0xffff);
	return (u_int16_t)(~(sum + (sum >> 16)));
}

void print_as_bytes (unsigned char* buff, ssize_t length) {
	for (ssize_t i = 0; i < length; i++, buff++) {
		printf ("%.2x ", *buff);	
    }
}

u_int16_t get_response_id(const char *buff) {
	
}

u_int16_t get_response_seq(const char *buff) {

}