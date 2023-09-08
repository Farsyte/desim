#pragma once
#include "cpu8080.h"
#include "ctl8228.h"
#include "dec8080.h"
#include "edge.h"
#include "gen8224.h"
#include "ram8107x8.h"
#include "rom8316.h"
#include "sio8080.h"

// sim8080 is a module that packages other modules
// into a viable 8080 based system.

struct sSys8080;

typedef struct sSys8080 {
    const char         *name;

    pEdge               OSC;

    Edge                RESIN_;
    Edge                RDYIN;
    Edge                HOLD;
    Edge                INT;

    Gen8224             gen;
    Ctl8228             ctl;
    Cpu8080             cpu;
    Dec8080             dec;

#define	SIO_PORT	128
    Sio8080             sio;

    Rom8316             rom[4];
    Ram8107x8           ram[4];

    // See Sys8080.c for the description of when Sys8080
    // asserts SHADOW_ENA and when it releases it.
    Edge                SHADOW_ENA;

}                  *pSys8080, Sys8080[1];

extern void         Sys8080_init(Sys8080);
extern void         Sys8080_linked(Sys8080);
extern void         Sys8080_down(Sys8080);
extern void         Sys8080_bist();
extern void         Sys8080_bench();
