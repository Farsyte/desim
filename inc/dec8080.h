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

    pEdge               SHADOW_ENA;             // when high, we are shadowing
    pEdge               SHADOW_SEL;             // enable signal shadowning page[0]

    pEdge               page[DEC8080_PAGES];
    pEdge               devi[DEC8080_PORTS];
    pEdge               devo[DEC8080_PORTS];

    pEdge               hidden_ram;             // bookkeeping: page[0] being shadowed
    pEdge               enabled;                // bookkeeping: enable being asserted

}                  *pDec8080, Dec8080[1];

extern void         Dec8080_init(Dec8080);
extern void         Dec8080_linked(Dec8080);

extern pEdge        Dec8080_take(Dec8080, int which_page, Edge e);

extern void         Dec8080_bist();
