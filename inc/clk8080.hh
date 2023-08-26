#pragma once

// Clock Generation for the Intel 8080A CPU
//
// This is based on Figure 3-3 of the MCS
// extended to include the services that
// normally are provided by the 8224 and 8228.
//
// Requirements:
// - call Clk8080::tick() for each cycle of
//   the master oscillator.
// Things we do:
// - drive OSC PHI1 PHI2 PHI1A at the right times
// - use PHI1A to latch RESET READY HOLD INT
// - use RESET SYNC PHI1A to generate /STSTB
// - use /STSTB to latch STATUS
// - use DBIN /WR HLDA from CPU
// - observe /BUSEN from environment
// - drive /MEMR /MEMW /IOR /IOW /INTA

// so this not only provides OSC PHI1 PHI2
// but also PHI1A, and provides synchronization
// for RESET READY HOLD and INT.
//
// Using SYNC from the CPU, we provide /STSTB,
// and we take on the 8228 task of latching
// the STATUS word from the data bus.

#include "byte.hh"
#include "edge.hh"
#include "module.hh"
#include "tau.hh"

class Clk8080 : public Module {
public:
    Edge OSC; // oscillator output for other uses
    Edge PHI1; // output: phase 1 clock for 8080
    Edge PHI2; // output: phase 2 clock for 8080
    Edge PHI1A; // internal timing signal (clock for latches below)

    // The /RESIN pin is an active low pin that rises slowly over time,
    // and is conditioned through a Schmitt trigger. It is latched
    // into a flip-flop by Φ₂D which happens during phase 5. The /Q
    // output of the flip-flop becomes RESET.

    Edge* RESIN; // ACTIVE LOW input: unsynchronized reset request
    Edge RESET; // output: synchronized RESET (latched on PHI2)

    // The RDYIN pin is an active high pin that is digital but is not
    // properly synchronized for use by the 8080. It is latched into a
    // flip-flop by Φ₂D which happens during phase 5. The Q output of
    // the flip-flop becomes READY.

    Edge* RDYIN; // input: unsynchronized ready request
    Edge READY; // output: synchronized RESET (latched on PHI2)

    // The DMARQ pin is an active high pin that is digital but is not
    // properly synchronized for use by the 8080. It is latched into a
    // flip-flop by Φ₂D which happens during phase 5. The Q output of
    // the flip-flop becomes HOLD.

    Edge* DMARQ; // input: unsynchronized DMA request
    Edge HOLD; // output: synchronized HOLD for DMA

    // The INTRQ pin is an active high pin that is digital but is not
    // properly synchronized for use by the 8080. It is latched into a
    // flip-flop by Φ₂D which happens during phase 5. The Q output of
    // the flip-flop becomes INT.

    Edge* INTRQ; // input: unsynchronized DMA request
    Edge INT; // output: synchronized HOLD for DMA

    // The processor asserts SYNC when it sees Φ₂ rising in the first
    // state of a machine cycle, and releases it on the next such
    // edge. The 8224 issues /STSTB (active low) when SYNC is active
    // and we are in phase 9. It also asserts /STSTB whenever the
    // RESET signal is active.

    Edge* SYNC; // input: start of machine cycle
    Edge STSTB; // output: status byte strobe

    // We observe the CPU Data Bus.
    Byte* D; // input: CPU Data Bus

    Edge* DBIN; // DBIN
    Edge* WR; // WR*
    Edge* HLDA; // HLDA

    Edge* BUSEN; // Bus Enable, active low

    // Marked as "Control Bus" in 8228 data sheet
    Edge INTA;
    Edge MEMR;
    Edge MEMW;
    Edge IOR;
    Edge IOW;

    // Clk8080::linked() is the required Module method that is called
    // by the simulation after all of the Edge objects have been
    // linked up. This method will verify all is well, and finish up
    // any remaining initialization of the module (possibly using the
    // state of any signals that have been connected).

    virtual void linked() = 0;

    // Clk8080::tick() is present for modules that need to be called
    // at a specific future time. When called, tick() should examine
    // the global TAU value, perform any state updates for that time,
    // and return the next TAU value at which they need to do a
    // time-based update step.
    //
    // Note that only a few very specific module have tick() functions,
    // and the executive code that configures a simulation with such
    // modules must contain specific main loop code that manages making
    // the proper tick() calls.
    //
    // The framework is organized around simulations that only create
    // one instance of one module that has tick() -- keeping a general
    // priority queue of which of several modules to call, and what TAU
    // value to use, imposes a runtime load that we want to avoid when
    // it is not absolutely necessary.

    virtual tau_t tick() = 0;

    // Create Clk8080 modules by calling Clk8080::create(name)
    // to keep private details out of the .hh file.

    static Clk8080* create(const char* name);

    static void bist();

protected:
    Clk8080(const char*); // please use Clk8080::create(name)
    virtual ~Clk8080();
};
