#include "ram8107x8.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "stub.h"
#include "util.h"

static void         rd_rise(Ram8107x8 ram);
static void         wr_rise(Ram8107x8 ram);

void Ram8107x8_init(Ram8107x8 ram)
{
    assert(ram);
    assert(ram->name);
    assert(ram->name[0]);

    ram->RD->name = format("%s_RD", ram->name);
    Edge_lo(ram->RD);

    ram->WR->name = format("%s_WR", ram->name);
    Edge_lo(ram->WR);

    assert(RAM8107x8_BYTES == sizeof(ram->mem));
    memset(ram->mem, 0, sizeof ram->mem);

    EDGE_RISE(ram->RD, rd_rise, ram);
    EDGE_RISE(ram->WR, wr_rise, ram);
}

void Ram8107x8_linked(Ram8107x8 ram)
{
    assert(ram->Addr);
    assert(ram->Data);

    // TODO what if we want a WAIT cycle?
}

static void rd_rise(Ram8107x8 ram)
{
    *ram->Data = ram->mem[RAM8107x8_AMASK & *ram->Addr];
}

static void wr_rise(Ram8107x8 ram)
{
    ram->mem[RAM8107x8_AMASK & *ram->Addr] = *ram->Data;
}
