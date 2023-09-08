#pragma once
#include "edge.h"

typedef struct {
    const char         *name;
    Edge                WR;
}                  *pCon8080, Con8080[1];

extern void         Con8080_init(Con8080);
extern void         Con8080_linked(Con8080);

extern void         Con8080_bist();
extern void         Con8080_bench();
