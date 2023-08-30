#include "sys8080.hh"

//#include <cassert>
#include <cstdio>
//#include <cstdlib>

//#include "edge.hh"
#include "util.hh"

class Sys8080impl : public Sys8080 {
public:
    Sys8080impl(const char* name);
    virtual void linked();
};

Sys8080impl::Sys8080impl(const char* name)
    : Sys8080(name)
{
    CLK.OSC = &OSC;

    CLK.RESIN = &RESIN;
    CLK.RDYIN = &RDYIN;
    CLK.DMARQ = &DMARQ;
    CLK.INTRQ = &INTRQ;

    CPU.PHI1  = &CLK.PHI1;  // 8224 phase 1 clock
    CPU.PHI2  = &CLK.PHI2;  // 8224 phase 2 clock
    CPU.RESET = &CLK.RESET; // 8224 synchronized RESET
    CPU.READY = &CLK.READY; // 8224 synchronized READY
    CPU.HOLD  = &CLK.HOLD;  // synchronized HOLD similar to above
    CPU.INT   = &CLK.INT;   // synchronized INT similar to above
    CLK.SYNC  = &CPU.SYNC;  // 8224 sync pulse for T1 and status word
    CLK.D     = &CPU.Data;  // 8228 cpu data bus
    CLK.DBIN  = &CPU.DBIN;  // 8228 data bus driven by cpu
    CLK.WR    = &CPU.WR;    // 8228 data bus driven to cpu
    CLK.HLDA  = &CPU.HLDA;  // 8228 hold state
    DEC.Addr  = &CPU.Addr;  // 8080 address bus
    DEC.MEMR  = &CLK.MEMR;  // 8228 control bus memory read
    DEC.MEMW  = &CLK.MEMW;  // 8228 control bus memory write
    DEC.IOR   = &CLK.IOR;   // 8228 control bus device read
    DEC.IOW   = &CLK.IOW;   // 8228 control bus device write

    CLK.linked();
    CPU.linked();
    DEC.linked();

    // Each ROM is 1024x8 bits. Add eight of them in the top 8 pages
    // of memory, then shadow them down into the bottom 8 pages.

    for (int i = 0; i < 8; ++i) {
        Rom8080* rom = ROMs[i];

        rom->Addr  = &CPU.Addr;
        rom->Data  = &CPU.Data;
        rom->RDYIN = &RDYIN;
        rom->DBIN  = &CPU.DBIN;
        rom->linked();

        // These live in the top 8K of the address space
        DEC.ROM[i + 56] = &rom->ENABLE;

        // And temporarily also in the bottom 8K
        DEC.ROM[i] = &rom->ENABLE;
    }

    // Each RAM is 4096x8 bits. Add sixteen of them, covering the
    // whole 64 KiB of the address space. Not all of this will be
    // accessable until the simulation disconnects the ROMs from
    // the address decode module.

    for (int i = 0; i < 16; ++i) {
        Ram8080* ram = RAMs[i];

        ram->Addr  = &CPU.Addr;
        ram->Data  = &CPU.Data;
        ram->RDYIN = &RDYIN;
        ram->DBIN  = &CPU.DBIN;
        ram->WR    = &CPU.WR;
        ram->linked();

        DEC.RAM[i] = &ram->ENABLE;
    }
}

void Sys8080impl::linked()
{
}

Sys8080* Sys8080::create(const char* name)
{
    return new Sys8080impl(name);
}

Sys8080::~Sys8080() { }
Sys8080::Sys8080(const char* name)
    : Module(name)
    , CLK(*Clk8080::create(format("%s.clk1", name)))
    , CPU(*Cpu8080::create(format("%s.cpu1", name)))
    , DEC(*Dec8080::create(format("%s.dec1", name)))
{
    for (int i = 0; i < 8; ++i)
        ROMs[i] = Rom8080::create(format("%s.rom%d", name, i + 1));
    for (int i = 0; i < 16; ++i)
        RAMs[i] = Ram8080::create(format("%s.ram%d", name, i + 1));
}
