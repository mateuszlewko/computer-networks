#define ERROR_EXIT(msg) do { \
    perror(msg);             \
    exit(EXIT_FAILURE);      \
} while (0)

#define MIN(a,b) (((a)<(b))?(a):(b))
