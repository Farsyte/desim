#include "tau.h"
#include <stdio.h>
#define STUB(...)                     \
    do {                              \
        fprintf(stderr, "%s:%d: ",    \
            __FILE__, __LINE__);      \
        fprintf(stderr, __VA_ARGS__); \
        fprintf(stderr, "\n");        \
    } while (0)
