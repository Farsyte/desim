#pragma once
#include "cpu8080.h"
#include "ctl8228.h"
#include "dec8080.h"
#include "edge.h"
#include "gen8224.h"

// Very approximate mirror of the 8316,
// a 8192-bit read only memory, with a small
// amount of external logic so that the simulation
// module inputs are simpler to connect to
// the rest of the Sys8080 simulation.

struct sRom8316;

#define ROM8316_BYTES	2048
#define ROM8316_AMASK	03777

typedef struct sRom8316 {
    const char         *name;

    pReg16              Addr;
    pReg8               Data;

    Edge                RD;

    Byte                mem[ROM8316_BYTES];

}                  *pRom8316, Rom8316[1];

extern void         Rom8316_init(Rom8316);
extern void         Rom8316_linked(Rom8316);
extern void         Rom8316_bist();
extern void         Rom8316_bench();
