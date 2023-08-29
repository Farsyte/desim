#include "cpu8080.hh"

#include <cassert>
#include <cstdio>
#include <cstdlib>

#include <functional>

#include "8080_status.hh"
#include "action.hh"
#include "tau.hh"

#include "link_assert.hh"
#include "stub.hh"

class Cpu8080impl : public Cpu8080 {
public:
    Cpu8080impl(const char* name)
        : Cpu8080(name)
    {
        // Edge_RISE(&OWNED_EDGE, fn);
    }

    virtual void linked()
    {
        // assert all our signals are linked
        LINK_ASSERT(PHI1);
        LINK_ASSERT(PHI2);
        LINK_ASSERT(RESET);
        LINK_ASSERT(READY);

        LINK_ASSERT(HOLD);
        LINK_ASSERT(INT);

        // assert our signals are in the initial state
        assert(RESET->get());
        assert(!READY->get());

        Edge_RISE(RESET, reset_rise);
        Edge_FALL(RESET, reset_fall);

        Edge_RISE(PHI1, phi1_rise);
        Edge_FALL(PHI1, phi1_fall);
        Edge_RISE(PHI2, phi2_rise);
        Edge_FALL(PHI2, phi2_fall);

        reset_rise();
    }

    // Architectural State of the Chip

    Byte ACT;  // accumulator latch
    Byte TMP;  // ALU temp reg
    Byte DBL;  // data bus buffer/latch
    Byte IR;   // instruction register
    Byte Z, W; // temp regs

protected:
    enum phase_e {
        PHI1_RISE,
        PHI1_FALL,
        PHI2_RISE,
        PHI2_FALL
    };

    char                         s_ch, s_ch_next, s_ch_last;
    const char*                  s_fn;
    const char*                  s_fn_next;
    std::function<void(phase_e)> s_fp, s_fp_next;

    //        STUB(" %6.2f μs: next is T%c, %s",
    //            (TAU * 0.001), ch, #fn);

#define NEXT_STATE(ch, fn)                       \
    do {                                         \
        s_ch_next = ch;                          \
        s_fp_next = [=](phase_e ph) { fn(ph); }; \
        s_fn_next = #fn;                         \
    } while (0)

    void phi1_rise()
    {
        s_ch = s_ch_next;
        s_fp = s_fp_next;
        s_fn = s_fn_next;
        // STUB(" %6.2f μs: State: T%c of %s", (TAU * 0.001), s_ch, s_fn);
        s_fp(PHI1_RISE);
    }

#define PH_PHI1_FALL
    void phi1_fall()
    {
        s_fp(PHI1_FALL);
    }

    void phi2_rise()
    {
        s_fp(PHI2_RISE);
    }

    void phi2_fall()
    {
        s_fp(PHI2_FALL);
    }

    void reset_rise()
    {
        // STUB(" %6.2f μs: at top of %s", (TAU * 0.001), __PRETTY_FUNCTION__);
        // reset_rise: switch internal state to the state we hold
        // while the RESET signal is active.
        NEXT_STATE('R', s_reset);
        s_ch = s_ch_next;
        s_fp = s_fp_next;
        s_fn = s_fn_next;

        // TODO Float the Addr bus
        // TODO Float the Data bus

        // De-assert the control signals
        SYNC.lo();
        DBIN.lo();
        WR.hi();
    }

    void reset_fall()
    {
        // STUB(" %6.2f μs: at top of %s", (TAU * 0.001), __PRETTY_FUNCTION__);
        // reset_fall: start transition out of RESET mode.
        NEXT_STATE('1', s_fetch);
    }

    void s_reset(phase_e)
    {
    }

    void s_fetch(phase_e ph)
    {
        // M1 T1 state
        switch (ph) {
        case PHI1_RISE:
            status = STATUS_FETCH;
            break;
        case PHI1_FALL:
            break;
        case PHI2_RISE:
            Addr = PC;
            Data = status;
            SYNC.hi();
            // STUB(" %6.2f μs: with Data=%03o assert SYNC", (TAU * 0.001), Data);
            break;
        case PHI2_FALL:
            NEXT_STATE('2', s_incpc);
            break;
        }
    }

    void s_incpc(phase_e ph)
    {
        // M1 T2 state
        switch (ph) {
        case PHI1_RISE:
            break;
        case PHI1_FALL:
            break;
        case PHI2_RISE:
            // STUB(" %6.2f μs: release SYNC, float Data, assert DBIN", (TAU * 0.001));
            SYNC.lo();
            PC++;
            Data = 0377;
            DBIN.hi();
            break;
        case PHI2_FALL:
            if (READY->get()) {
                NEXT_STATE('3', s_tmpir);
            } else {
                // STUB(" %6.2f μs: insert TW state", (TAU * 0.001));
                NEXT_STATE('W', s_op_tw);
            }
            break;
        }
    }

    void s_op_tw(phase_e ph)
    {
        // M1 TW state
        switch (ph) {
        case PHI1_RISE:
            break;
        case PHI1_FALL:
            break;
        case PHI2_RISE:
            break;
        case PHI2_FALL:
            if (READY->get()) {
                NEXT_STATE('3', s_tmpir);
            } else {
                // STUB(" %6.2f μs: continue TW state", (TAU * 0.001));
                NEXT_STATE('W', s_op_tw);
            }
            break;
        }
    }

    void s_tmpir(phase_e ph)
    {
        // M1 T3 state
        switch (ph) {
        case PHI1_RISE:
            break;
        case PHI1_FALL:
            break;
        case PHI2_RISE:
            // STUB(" %6.2f μs: latch %03o from %05o into IR, release DBIN",
            //      (TAU * 0.001), Data, Addr);
            IR = Data;
            DBIN.lo();
            break;
        case PHI2_FALL:
            // TODO select s_fp based on opcode
            NEXT_STATE('4', s_nopT4);
            break;
        }
    }

    void s_nopT4(phase_e ph)
    {
        // M1 T3 state
        switch (ph) {
        case PHI1_RISE:
            break;
        case PHI1_FALL:
            break;
        case PHI2_RISE:
            // This is, I think, also where we float the Addr bus.
            // STUB(" %6.2f μs: execute NOP instruction", (TAU * 0.001));
            break;
        case PHI2_FALL:
            NEXT_STATE('1', s_fetch);
            break;
        }
    }

    Byte status;
};

Cpu8080* Cpu8080::create(const char* name)
{
    return new Cpu8080impl(name);
}

Cpu8080::Cpu8080(const char* name)
    : Module(name)
    , WR(1)
{
}
