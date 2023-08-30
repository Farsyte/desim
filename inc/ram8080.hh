#pragma once

#include "byte.hh"
#include "edge.hh"
#include "module.hh"
#include "word.hh"

// Simulated RAM for the simulated 8080 based system
class Ram8080 : public Module {
public:
    // State visible in hardware outside the chip

    // Address and Data bus: simple storage of state that the
    // simulation just allows everyone to access.
    Word* Addr; // A₁₅..A₀ 16-bit address bus
    Byte* Data; // D₇..D₀ 8-bit bidirectional data bus

    Edge ENABLE; // from dec8080: decoded enable in my memory range

    Edge* RDYIN; // into clk8080: set low to request a wait state
    Edge* DBIN;  // from cpu8080: "accepting Data now"
    Edge* WR;    // from cpu8080: "providing Data now"

    virtual void linked() = 0;

    static void     bist();
    static Ram8080* create(const char* name);

protected:
    Ram8080(const char*); // please use Ram8080::create(name)
};
