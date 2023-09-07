#pragma once
#include "tau.h"
#include <stdio.h>
#include <stdlib.h>

#define STUB(...)                     \
    do {                              \
        fprintf(stderr, "%s:%d: ",    \
            __FILE__, __LINE__);      \
        fprintf(stderr, __VA_ARGS__); \
        fprintf(stderr, "\n");        \
    } while (0)

#define FAIL(...)                     \
    do {                              \
        fprintf(stderr, "%s:%d: ",    \
            __FILE__, __LINE__);      \
        fprintf(stderr, __VA_ARGS__); \
        fprintf(stderr, "\n");        \
        abort();                      \
    } while (0)
