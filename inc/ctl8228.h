#pragma once
#include "edge.h"

typedef struct {
    const char         *name;

    Byte               *Data;

    pEdge               STSTB_;
    pEdge               DBIN;
    pEdge               WR_;
    pEdge               HLDA;

    Edge                MEMR_;
    Edge                MEMW_;
    Edge                IOR_;
    Edge                IOW_;
    Edge                INTA_;

    Byte                status;

}                  *pCtl8228, Ctl8228[1];

extern void         Ctl8228_init(Ctl8228);
extern void         Ctl8228_linked(Ctl8228);
extern void         Ctl8228_bist();
