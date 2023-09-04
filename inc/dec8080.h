#pragma once

#include "edge.h"

#define DEC8080_PAGES	64
#define DEC8080_PORTS	256

typedef struct {
    const char         *name;

    pReg16              Addr;

    // Control signals from Ctl8228
    pEdge               MEMR_;
    pEdge               MEMW_;
    pEdge               IOR_;
    pEdge               IOW_;

    // When SHADOW_ENA is high, memory page 0 decodes are
    // diverted to SHADOW_ENA.

    pEdge               SHADOW_ENA;
    pEdge               SHADOW_SEL;

    pEdge               page[DEC8080_PAGES];
    pEdge               devi[DEC8080_PORTS];
    pEdge               devo[DEC8080_PORTS];

    pEdge               hidden_ram;
    pEdge               enabled;

}                  *pDec8080, Dec8080[1];

extern unsigned     Dec8080_debug;

extern pEdge        Dec8080_take(Dec8080, int which_page, Edge e);
extern void         Dec8080_init(Dec8080);
extern void         Dec8080_linked(Dec8080);
extern void         Dec8080_bist();
