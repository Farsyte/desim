#pragma once
#include "edge.h"

#define DEC8080_PAGES	64
#define DEC8080_PORTS	256

typedef struct {
    const char         *name;

    pReg16              Addr;

    pEdge               MEMR_;
    pEdge               MEMW_;
    pEdge               IOR_;
    pEdge               IOW_;

    pEdge               DBIN;
    pEdge               WR_;

    pEdge               SHADOW_ENA;             // when high, we are shadowing
    pEdge               SHADOW_CE;              // shadow memory "ce" signal

    pEdge               memr[DEC8080_PAGES];
    pEdge               memw[DEC8080_PAGES];
    pEdge               devr[DEC8080_PORTS];
    pEdge               devw[DEC8080_PORTS];

    pEdge               asserted;               // which "ce" signal Dec asserted
    pEdge               shadowed;               // which "ce" signal is being shadowed
}                  *pDec8080, Dec8080[1];

extern void         Dec8080_init(Dec8080);
extern void         Dec8080_linked(Dec8080);

extern void         Dec8080_bist();
