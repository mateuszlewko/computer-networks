#define ERROR_EXIT(msg) do { \
    perror(msg);             \
    exit(EXIT_FAILURE);      \
} while (0)

typedef unsigned char byte;
