#pragma once

#include "clock.h"
#include "ctl8228.h"
#include "edge.h"
#include "gen8224.h"

typedef enum {
    UNPHASED, PHI1_RISE, PHI2_RISE, PHI2_FALL
} Cpu8080_phase;

struct sCpu8080;

typedef void        (*Cpu8080_state)(struct sCpu8080 *, Cpu8080_phase);

typedef struct sCpu8080 {
    const char         *name;

    pEdge               PHI1;
    pEdge               PHI2;
    pEdge               RESET;
    pEdge               READY;
    pEdge               HOLD;
    pEdge               INT;

    Reg16               Addr;
    Reg8                Data;

    Edge                INTE;
    Edge                DBIN;
    Edge                WR_;
    Edge                SYNC;
    Edge                WAIT;
    Edge                HLDA;

    Reg16               PC;

    Reg8                IR;

    Cpu8080_state       state;
    Cpu8080_state       state_next;

    Edge                RETM1;

    Cpu8080_state       M1T1;
    Cpu8080_state       M1T4[0400];

    Cpu8080_state       fetch;

    Edge                INT_RST;

}                  *pCpu8080, Cpu8080[1];

extern unsigned     Cpu8080_debug;

extern void         Cpu8080_init(Cpu8080);
extern void         Cpu8080_linked(Cpu8080);
extern void         Cpu8080_bist();
extern void         Cpu8080_bench();
