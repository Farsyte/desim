#include "rom8316.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "stub.h"
#include "util.h"

static void         rd_rise(Rom8316 rom);

void Rom8316_init(Rom8316 rom)
{
    assert(rom);
    assert(rom->name);
    assert(rom->name[0]);

    rom->RD->name = format("%s_RD", rom->name);
    Edge_lo(rom->RD);

    assert(ROM8316_BYTES == sizeof(rom->mem));
    memset(rom->mem, 0, sizeof rom->mem);
}

void Rom8316_linked(Rom8316 rom)
{
    assert(rom->Addr);
    assert(rom->Data);

    EDGE_RISE(rom->RD, rd_rise, rom);

    // TODO what if we want a WAIT cycle?
}

static void rd_rise(Rom8316 rom)
{
    *rom->Data = rom->mem[ROM8316_AMASK & *rom->Addr];
}
