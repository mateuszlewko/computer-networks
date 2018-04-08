#include <netinet/ip.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <limits.h>

char *int2bin(unsigned n, char *buf)
{
    #define BITS (sizeof(n) * CHAR_BIT)

    static char static_buf[BITS + 1];
    int i;

    if (buf == NULL)
        buf = static_buf;

    for (i = BITS - 1; i >= 0; --i) {
        buf[i] = (n & 1) ? '1' : '0';
        n >>= 1;
    }

    buf[BITS] = '\0';
    return buf;

    #undef BITS
}

int main(int argc, char* argv[])
{
	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0) {
		fprintf(stderr, "socket error: %s\n", strerror(errno)); 
		return EXIT_FAILURE;
	}

	if (argc != 2) {
		fprintf (stderr, "usage: %s number_of_datagrams\n", argv[0]);
		return EXIT_FAILURE;
	}

	struct sockaddr_in server_address;
	bzero (&server_address, sizeof(server_address));
	server_address.sin_family      = AF_INET;
	server_address.sin_port        = htons(12345);
	inet_pton(AF_INET, "127.0.0.255", &server_address.sin_addr);
	
	printf("%s\n", int2bin(server_address.sin_addr.s_addr, NULL));

	char* message = "Hello server!";
	ssize_t message_len = strlen(message);
	for (int i = 0; i < atoi(argv[1]); i++) {
		if (sendto(sockfd, message, message_len, 0, (struct sockaddr*) &server_address, sizeof(server_address)) != message_len) {
			fprintf(stderr, "sendto error: %s\n", strerror(errno)); 
			return EXIT_FAILURE;		
		}
	}

	close (sockfd);
	return EXIT_SUCCESS;
}
	
