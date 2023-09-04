#pragma once

#include "clock.h"
#include "ctl8228.h"
#include "edge.h"
#include "gen8224.h"

// Could use an enum for the phase, but then
// all the switch statements would have to
// have cases for all the phases. This is
// normally a useful warning -- adding an
// entry to an enum often means adding code
// for it to all switches -- but in this case,
// if we add more phases, then we want existing
// switches to just do nothing.

typedef int         Cpu8080_phase;
#define	PHI1_RISE	1
#define	PHI2_RISE	3
#define	PHI2_FALL	7

// DESim does not (yet?) track whether a bus
// is floating or not. To make it easy to find
// places where a module "floats" a bus, provide
// a macro for a value to store.
#define BUS_FLOAT	0

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

}                  *pCpu8080, Cpu8080[1];

extern unsigned     Cpu8080_debug;

extern void         Cpu8080_init(Cpu8080);
extern void         Cpu8080_linked(Cpu8080);
extern void         Cpu8080_bist();
extern void         Cpu8080_bench();
