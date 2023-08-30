#pragma once

#include "edge.hh"
#include "module.hh"
#include "word.hh"

// Address Space Decoder for the simulated 8080 based system
class Dec8080 : public Module {
public:
    // Address and Data bus: simple storage of state that the
    // simulation just allows everyone to access.
    Word* Addr; // A₁₅..A₀ 16-bit address bus

    Edge* MEMR;
    Edge* MEMW;
    Edge* IOR;
    Edge* IOW;

    // Edge for each KiB of memory, can be NULL, mutable.
    // Separte links for RAM and ROM to make it easy to
    // have ROM temporarily take the place of RAM.
    Edge* ROM[64]; // ROM enables for each 1 KiB
    Edge* RAM[16]; // RAM enables for each 4 KiB

    // Edge for each I/O port, can be NULL, mutable.
    Edge* DEV[256];

    virtual void linked() = 0;

    static void     bist();
    static Dec8080* create(const char* name);

protected:
    Dec8080(const char*); // please use Dec8080::create(name)
};
