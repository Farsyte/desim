#pragma once
#include "cpu8080.h"
#include "ctl8228.h"
#include "dec8080.h"
#include "edge.h"
#include "gen8224.h"

// Very approximate mirror of the eight 8107 chips, each a 4096-bit
// read-write memory, with a small amount of external logic so that
// the simulation module inputs are simpler to connect to the rest of
// the Sys8080 simulation.
//
// The 8107 is a dynamic ram, and each row (selected by the low six
// bits of the address) must be refreshed at least once every 2ms.
//
// The simulation does not (YET!) model this need.

struct sRam8107x8;

#define RAM8107x8_BYTES	4096
#define RAM8107x8_AMASK	07777

typedef struct sRam8107x8 {
    const char         *name;

    pReg16              Addr;
    pReg8               Data;

    Edge                RD;
    Edge                WR;

    Byte                mem[RAM8107x8_BYTES];

}                  *pRam8107x8, Ram8107x8[1];

extern void         Ram8107x8_init(Ram8107x8);
extern void         Ram8107x8_linked(Ram8107x8);
extern void         Ram8107x8_bist();
extern void         Ram8107x8_bench();
