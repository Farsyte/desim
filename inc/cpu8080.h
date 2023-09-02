#pragma once

#include "clock.h"
#include "ctl8228.h"
#include "edge.h"
#include "gen8224.h"

typedef enum {
    UNPHASED, PHI1_RISE, PHI2_RISE, PHI2_FALL
} Cpu8080_phase;

struct sCpu8080;

typedef void        Cpu8080_state(struct sCpu8080 *, Cpu8080_phase);

typedef struct sCpu8080 {
    const char         *name;

    // Changes from the 8080:

    // External Hardware Connections

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

    // Programmer Visible State
    //
    // TODO create as needed.
    // Expected entries:
    //    Reg8                B, C, D, E, H, L;
    //    Reg8                A;
    Reg16               PC;
    //    Reg16               SP;
    //    Reg1                 Zero, Carry, Sign, Parity, AuxCarry;

    // Architectural State that is not programmer visible
    //
    // TODO create as needed.
    // Expected entries:
    //    Reg8                W, Z;
    //    Reg8                ACT, TMP;
    Reg8                IR;

    // Internal State used by the simulation
    //
    // TODO create as needed.
    // Expected entries:
    //    pReg8               sss;
    //    pReg8               ddd;
    //    pReg8               rph;
    //    pReg8               rpl;

    Cpu8080_state      *state_next;
    Cpu8080_state      *state;
}                  *pCpu8080, Cpu8080[1];

extern unsigned     Cpu8080_debug;

extern void         Cpu8080_init(Cpu8080);
extern void         Cpu8080_linked(Cpu8080);
extern void         Cpu8080_bist();
