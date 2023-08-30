#pragma once

// Microcomputer System using the Intel 8080 CPU

#include "clk8080.hh"
#include "cpu8080.hh"
#include "dec8080.hh"
#include "module.hh"
#include "ram8080.hh"
#include "rom8080.hh"

#include "edge.hh"
#include "tau.hh"

class Sys8080 : public Module {
public:
    static void bist();

    static Sys8080* create(const char* name);

    Edge OSC; // system timebase. rising edges drive the world.

    // TODO allow open collector behavior.
    Edge RESIN; // reset input.
    Edge RDYIN;
    Edge DMARQ;
    Edge INTRQ;

    Clk8080& CLK;      // clock generator, status latch, control bus
    Cpu8080& CPU;      // central processing unit
    Dec8080& DEC;      // address decoding
    Rom8080* ROMs[8];  // eight 1024-by-8 bit ROMs
    Ram8080* RAMs[16]; // sixteen 4096-by-8 bit RAMs

    virtual void linked() = 0;

    Sys8080(const char*); // please use Sys8080::create(name)
    virtual ~Sys8080();
};
