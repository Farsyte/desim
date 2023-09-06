#include "sys8080.h"
#include <assert.h>
#include "util.h"

void Sys8080_init(Sys8080 sys)
{
    assert(sys);
    assert(sys->name);
    assert(sys->name[0]);

    sys->RESIN_->name = format("%s./RESIN", sys->name);
    sys->RDYIN->name = format("%s.RDYIN", sys->name);
    sys->HOLD->name = format("%s.HOLD", sys->name);
    sys->INT->name = format("%s.INT", sys->name);

    sys->gen->name = format("%s.gen", sys->name);
    sys->ctl->name = format("%s.ctl", sys->name);
    sys->cpu->name = format("%s.cpu", sys->name);
    sys->dec->name = format("%s.dec", sys->name);

    Edge_init(sys->RESIN_);
    Edge_init(sys->RDYIN);
    Edge_init(sys->HOLD);
    Edge_init(sys->INT);

    Gen8224_init(sys->gen);
    Ctl8228_init(sys->ctl);
    Cpu8080_init(sys->cpu);
    Dec8080_init(sys->dec);
}

void Sys8080_linked(Sys8080 sys)
{
    assert(sys);
    assert(sys->name);
    assert(sys->name[0]);

    pEdge               OSC = sys->OSC;

    pEdge               RESIN_ = sys->RESIN_;
    pEdge               RDYIN = sys->RDYIN;
    pEdge               HOLD = sys->HOLD;
    pEdge               INT = sys->INT;

    pGen8224            gen = sys->gen;
    pCtl8228            ctl = sys->ctl;
    pCpu8080            cpu = sys->cpu;
    pDec8080            dec = sys->dec;

    assert(OSC);
    assert(OSC->name);
    assert(OSC->name[0]);

    gen->OSC = OSC;
    gen->RESIN_ = RESIN_;
    gen->RDYIN = RDYIN;
    gen->SYNC = cpu->SYNC;

    ctl->Data = cpu->Data;

    ctl->STSTB_ = gen->STSTB_;
    ctl->DBIN = cpu->DBIN;
    ctl->WR_ = cpu->WR_;
    ctl->HLDA = cpu->HLDA;

    cpu->PHI1 = gen->PHI1;
    cpu->PHI2 = gen->PHI2;
    cpu->RESET = gen->RESET;
    cpu->READY = gen->READY;
    cpu->HOLD = HOLD;
    cpu->INT = INT;

    dec->Addr = cpu->Addr;
    dec->MEMR_ = ctl->MEMR_;
    dec->MEMW_ = ctl->MEMW_;
    dec->IOR_ = ctl->IOR_;
    dec->IOW_ = ctl->IOW_;

    // dec->SHADOW_ENA = SHADOW_ENA;
    // dec->SHADOW_SEL = SHADOW_SEL;

    // cpu->WAIT is not connected to anything.
    // cpu->INTE is not connected to anything.
    // cpu->INTA_ is not connected to anything.

    Gen8224_linked(gen);
    Ctl8228_linked(ctl);
    Cpu8080_linked(cpu);
    Dec8080_linked(dec);
}
