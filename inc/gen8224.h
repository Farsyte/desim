#pragma once

#include "edge.h"

typedef struct {
    const char         *name;

    // Change from 8224: someone else owns OSC,
    // and we expect them to make it rise and fall.
    pEdge               OSC;

    Edge                PHI1;
    Edge                PHI2;
    Edge                PHI1A;

} Gen8224[1];

extern unsigned     Gen8224_debug;

extern void         Gen8224_init(Gen8224);

extern void         Gen8224_bist();
