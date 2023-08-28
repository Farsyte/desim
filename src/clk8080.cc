#include "clk8080.hh"
//#include "edge.hh"
//#include "util.hh"

#include "8080_status.hh"
#include "link_assert.hh"

#include <cassert>
#include <cstdio>
//#include <cstdlib>

class Clk8080impl : public Clk8080 {
public:
    Clk8080impl(const char* name);
    virtual void linked();

protected:
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
    union {
        struct {
#pragma GCC diagnostic pop
            STATUS_BIT_DECLS;
            // bits from low to high, on little-endian simulation hosts.
            //            unsigned sINTA  : 1; // Interrupt Acknowledge
            //            unsigned sWO    : 1; // Write Output (active low)
            //            unsigned sSTACK : 1; // Stack Memory Access
            //            unsigned sHLTA  : 1; // Halt Acknowledge
            //            unsigned sOUT   : 1; // Output Write
            //            unsigned sM1    : 1; // Instruction Fetch (including Interrupt Service)
            //            unsigned sINP   : 1; // Input Read
            //            unsigned sMEMR  : 1; // Meory Read
        };
        Byte sBYTE;
    };

    void on_osc_rise();
};

Clk8080* Clk8080::create(const char* name)
{
    return new Clk8080impl(name);
}

Clk8080::Clk8080(const char* name)
    : Module(name)
    , MEMR(1)
    , MEMW(1)
    , IOR(1)
    , IOW(1)
    , INTA(1)
{
}

// Initialize the implementation of the
// Clock Generation for the Intel 8080A CPU
//
// This is based on Figure 3-3 of the MCS
// extended to include the RESET signal
// to match the Intel 8224 capability.
//
// Generally, we can use this module to latch
// any incoming signals that the CPU wants to
// have change shortly after Φ₂ rises.

Clk8080impl::Clk8080impl(const char* name)
    : Clk8080(name)
//    , tau_next(TAU)
//    , tau_frac(0)
{
}

#define STATUS_RESET    0b00000010
#define STATUS_FETCH    0b10100010
#define STATUS_MREAD    0b10000010
#define STATUS_MWRITE   0b00000000
#define STATUS_SREAD    0b10000110
#define STATUS_SWRITE   0b00000100
#define STATUS_INPUTRD  0b01000010
#define STATUS_OUTPUTWR 0b00010000
#define STATUS_INTACK   0b00100011
#define STATUS_HALTACK  0b10001010
#define STATUS_INTACKW  0b00101011

//#include "link_assert.hh"
void Clk8080impl::linked()
{
    LINK_ASSERT(OSC); // system timebase

    LINK_ASSERT(RESIN); // ACTIVE LOW input: unsynchronized reset request
    LINK_ASSERT(RDYIN); // input: unsynchronized ready request
    LINK_ASSERT(DMARQ); // input: unsynchronized DMA request
    LINK_ASSERT(INTRQ); // input: unsynchronized DMA request
    LINK_ASSERT(SYNC);  // input: start of machine cycle

    LINK_ASSERT(D);    // input: CPU Data Bus
    LINK_ASSERT(DBIN); // input: DBIN from 8080
    LINK_ASSERT(WR);   // input: WR from 8080
    LINK_ASSERT(HLDA); // input: HLDA from 8080

    assert(!RESIN->get());
    assert(!RDYIN->get());
    assert(!RESET.get());
    assert(!READY.get());

    assert(WR->get());

    // Require that signals are in their proper initial states.

    assert(!RESIN->get());
    assert(!RESET.get());

    RESET.rise_cb([=] {
        STSTB.hi();
    });

    PHI1A.rise_cb([=] {
        RESET.inv(!RESIN->get());
        READY.set(RDYIN->get());
        HOLD.set(DMARQ->get());
        INT.set(INTRQ->get());

        if (!RESET.get())
            STSTB.lo();
        else if (SYNC->get())
            STSTB.lo();
    });

    PHI1A.fall_cb([=] {
        if (RESET.get())
            STSTB.hi();
    });

    // Fig 3-5 tells us:
    //   /INTA = nand(pDBIN, sINTA)
    //   /IOR  = nand(pDBIN, sINP)
    //   /MEMR = nand(pDBIN, sMEMR)
    //
    //   /IOW  = nand(not(pWR), OUT)
    //   /MEMW = nand(not(pWR), not(out));
    //
    // However, the 8228 timing differs:
    // it asserts /INTA /IOR /MEMR with
    // the falling edge of STSTB, and holds
    // it until DBIN rises.

    STSTB.fall_cb([=] {
        sBYTE = *D;

        // match 8228 waveforms:
        // assert these, if there is
        // going to be a DBIN.

        if (sINTA || !sHLTA) {
            MEMR.inv(sMEMR);
            IOR.inv(sINP);
            INTA.inv(sINTA);
        }
    });

    DBIN->rise_cb([=] {
        // match Figure 3-5
        MEMR.inv(sMEMR);
        IOR.inv(sINP);
        INTA.inv(sINTA);
    });

    DBIN->fall_cb([=] {
        MEMR.hi();
        IOR.hi();
        INTA.hi();
    });

    WR->fall_cb([=] {
        if (sOUT)
            IOW.lo();
        else
            MEMW.lo();
    });

    WR->rise_cb([=] {
        MEMW.hi();
        IOW.hi();
    });

    Edge_RISE(OSC, on_osc_rise);
}

void Clk8080impl::on_osc_rise()
{
    static unsigned phase      = 0;
    static unsigned next_phase = 1;

    phase = next_phase++;

    switch (phase) {
    case 1:
        PHI1.hi();
        break;
    case 2:
        break;
    case 3:
        PHI1.lo();
        break;
    case 4:
        PHI2.hi();
        break;
    case 5:
        PHI1A.hi();
        break;
    case 6:
        break;
    case 7:
        break;
    case 8:
        PHI2.lo();
        break;
    case 9:
        PHI1A.lo();
        next_phase = 1;
        break;
    }
}
