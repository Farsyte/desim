#pragma once

#include "byte.hh"
#include "edge.hh"
#include "module.hh"
#include "tau.hh"
#include "word.hh"

class Cpu8080 : public Module {
public:
    // Address and Data bus: simple storage of state that the
    // simulation just allows everyone to access.
    Byte D; // D₇..D₀ 8-bit bidirectional data bus
    Word A; // A₁₅..A₀ 16-bit address bus

    // Input signals from the 8224 clock generator
    Edge* PHI1;  // 8080 pin 22 input (from 8224) phase 1 clock
    Edge* PHI2;  // 8080 pin 15 input (from 8224) phase 2 clock
    Edge* RESET; // 8080 pin 12 input (from 8224) synchronized RESET
    Edge* READY; // 8080 pin 23 input (from 8224) synchronized READY input

    // Output signals to the 8224 clock generator
    Edge SYNC; // 8080 pin 19 output (to 8224) mark M1, indicate STATUS on D
    Edge WAIT; // 8080 pin 24 output (to 8224), is this RDYIN?

    // Output signals to the 8228 system controller and bus driver
    Edge DBIN; // 8080 pin 17 output (to 8228) data bus direction
    Edge WR;   // 8080 pin 18 output (to 8228) write strobe (active low)
    Edge HLDA; // 8080 pin 21 output (to 8228) hold acknowledge

    // Connections to the "outside world"
    Edge* HOLD; // 8080 pin 13 input (from world) System DMA Request
    Edge* INT;  // 8080 pin 14 input (from world) interrupt request
    Edge  INTE; // 8080 pin 16 output (to world) Interrupt Enable

    virtual void linked() = 0;

    static void     bist();
    static Cpu8080* create(const char* name);

protected:
    Cpu8080(const char*); // please use Cpu8080::create(name)
};
