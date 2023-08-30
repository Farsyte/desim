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
    Byte STATUS;
#define sINTA  (STATUS & STATUS_INTA)
#define sWO    (STATUS & STATUS_WO)
#define sSTACK (STATUS & STATUS_STACK)
#define sHLTA  (STATUS & STATUS_HLTA)
#define sOUT   (STATUS & STATUS_OUT)
#define sM1    (STATUS & STATUS_M1)
#define sINP   (STATUS & STATUS_INP)
#define sMEMR  (STATUS & STATUS_MEMR)

    void on_osc_rise();
};

Clk8080* Clk8080::create(const char* name)
{
    return new Clk8080impl(name);
}

Clk8080::Clk8080(const char* name)
    : Module(name)
    , RESET(1)
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

    // Require that signals are in their proper initial states.

    assert(!RESIN->get());
    assert(!RDYIN->get());
    assert(RESET.get());
    assert(!READY.get());
    assert(!STSTB.get());

    assert(WR->get());

    RESET.rise_cb([=] { STSTB.lo(); });
    RESET.fall_cb([=] { STSTB.hi(); });

    PHI1A.rise_cb([=] {
        RESET.set(!RESIN->get());
        READY.set(RDYIN->get());
        HOLD.set(DMARQ->get());
        INT.set(INTRQ->get());

        if (RESET.get())
            STSTB.lo();
        else if (SYNC->get())
            STSTB.lo();
    });

    PHI1A.fall_cb([=] {
        if (!RESET.get())
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
        STATUS = *D;

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
