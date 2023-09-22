#pragma once
#include "edge.h"

typedef struct {
    const char         *name;

    pEdge               OSC;

    pEdge               RESIN_;
    pEdge               RDYIN;
    pEdge               SYNC;

    Edge                PHI1;
    Edge                PHI2;

    Edge                RESET;
    Edge                READY;
    Edge                STSTB_;

    unsigned            _phase;

}                  *pGen8224, Gen8224[1];

extern void         Gen8224_init(Gen8224);
extern void         Gen8224_linked(Gen8224);

extern void         Gen8224_bist();
extern void         Gen8224_bench();
