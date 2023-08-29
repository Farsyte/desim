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
    clk.OSC = &OSC;

    clk.RESIN = &RESIN;
    clk.RDYIN = &RDYIN;
    clk.DMARQ = &DMARQ;
    clk.INTRQ = &INTRQ;

    cpu.PHI1  = &(clk.PHI1);  // 8224 phase 1 clock
    cpu.PHI2  = &(clk.PHI2);  // 8224 phase 2 clock
    cpu.RESET = &(clk.RESET); // 8224 synchronized RESET
    cpu.READY = &(clk.READY); // 8224 synchronized READY

    cpu.HOLD = &(clk.HOLD); // synchronized HOLD similar to above
    cpu.INT  = &(clk.INT);  // synchronized INT similar to above

    clk.SYNC = &(cpu.SYNC); // 8224 sync pulse for T1 and status word

    clk.D    = &(cpu.D);    // 8228 cpu data bus
    clk.DBIN = &(cpu.DBIN); // 8228 data bus driven by cpu
    clk.WR   = &(cpu.WR);   // 8228 data bus driven to cpu
    clk.HLDA = &(cpu.HLDA); // 8228 hold state

    clk.linked();
    cpu.linked();
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
    , clk(*Clk8080::create(format("%s.clk1", name)))
    , cpu(*Cpu8080::create(format("%s.cpu1", name)))
{
}
