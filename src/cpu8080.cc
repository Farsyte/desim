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

        init_s4();

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
        UNPHASED,
        PHI1_RISE,
        PHI1_FALL,
        PHI2_RISE,
        PHI2_FALL
    };

    typedef std::function<void(phase_e)> pf_t;

    class Pfn {
    public:
        Pfn()
            : fp([](phase_e) {})
            , fn("")
        {
        }
        Pfn(pf_t fp, const char* fn)
            : fp(fp)
            , fn(fn)
        {
        }
        pf_t        fp;
        const char* fn;
    };

    char s_ch, s_ch_next, s_ch_last;
    Pfn  s_pfn, s_pfn_next;
    Pfn  s4[0377];

#define PFN(fn) Pfn([=](phase_e ph) { fn(ph); }, #fn)

    void init_s4()
    {
        unsigned ns4 = sizeof s4 / sizeof s4[0];
        Pfn      ill = PFN(s4ill);
        for (unsigned i = 0; i < ns4; ++i)
            s4[i] = ill;

        s4[0] = PFN(s4nop);

        // Handle MOV between B,C,D,E,H,L
        Pfn movRR4 = PFN(s4movRR);
        for (unsigned ddd = 0; ddd < 6; ++ddd) {
            for (unsigned sss = 0; sss < 6; ++sss) {
                unsigned op = 0100 | (ddd << 3) | sss;
                s4[op]      = movRR4; // MOV r,r
            }
        }
        // Handle MOV from A to B,C,D,E,H,L
        Pfn movRA4 = PFN(s4movRA);
        for (unsigned ddd = 0; ddd < 6; ++ddd) {
            unsigned op = 0107 | (ddd << 3);
            s4[op]      = movRA4; // MOV r,A
        }
        // Handle MOV from B,C,D,E,H,L to A
        Pfn movAR4 = PFN(s4movAR);
        for (unsigned sss = 0; sss < 6; ++sss) {
            unsigned op = 0170 | sss;
            s4[op]      = movAR4; // MOV A,r
        }

        s4[0166] = PFN(s4hlt);
    }

#define NEXT_STATE_PFN(ch, pfn) \
    do {                     \
        s_ch_next  = ch;     \
        s_pfn_next = pfn;    \
        return;              \
    } while (0)

#define NEXT_STATE_FN(ch, fn) \
    NEXT_STATE_PFN(ch, PFN(fn))

    void phi1_rise()
    {
        s_ch  = s_ch_next;
        s_pfn = s_pfn_next;
        // STUB(" %6.2f μs: State: T%c of %s", (TAU * 0.001), s_ch, s_pfn.fn);
        s_pfn.fp(PHI1_RISE);
    }

    void phi1_fall() { s_pfn.fp(PHI1_FALL); }
    void phi2_rise() { s_pfn.fp(PHI2_RISE); }
    void phi2_fall() { s_pfn.fp(PHI2_FALL); }

    void reset_rise()
    {
        // reset_rise: switch internal state to the state we hold
        // while the RESET signal is active.

        sRreset(UNPHASED);

        // TODO Float the Addr bus
        // TODO Float the Data bus

        s_ch  = '*';
        s_pfn = Pfn([](phase_e) {}, "RESET_RISING");

        // STUB(" %6.2f μs: at top of %s", (TAU * 0.001), __PRETTY_FUNCTION__);

        NEXT_STATE_FN('R', sRreset);
    }

    void reset_fall()
    {
        // STUB(" %6.2f μs: at top of %s", (TAU * 0.001), __PRETTY_FUNCTION__);
        // reset_fall: start transition out of RESET mode.
        NEXT_STATE_FN('1', s1fetch);
    }

    void sRreset(phase_e)
    {
        // De-assert the control signals
        SYNC.lo();
        DBIN.lo();
        WR.hi();
    }

    void s1fetch(phase_e ph)
    {
        switch (ph) {
        case UNPHASED:
            break;
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
            NEXT_STATE_FN('2', s2incpc);
            break;
        }
    }

    void s2incpc(phase_e ph)
    {
        switch (ph) {
        case UNPHASED:
            break;
        case PHI1_RISE:
            break;
        case PHI1_FALL:
            break;
        case PHI2_RISE:
            // STUB(" %6.2f μs: release SYNC, float Data, assert DBIN", (TAU * 0.001));
            SYNC.lo();
            PC++;
            Data = 0377; // TODO represent FLOATING Data
            DBIN.hi();
            break;
        case PHI2_FALL:
            if (READY->get()) {
                NEXT_STATE_FN('3', s3tmpir);
            } else {
                // STUB(" %6.2f μs: insert TW state", (TAU * 0.001));
                NEXT_STATE_FN('W', sWfetch);
            }
            break;
        }
    }

    void sWfetch(phase_e ph)
    {
        switch (ph) {
        case UNPHASED:
            break;
        case PHI1_RISE:
            break;
        case PHI1_FALL:
            break;
        case PHI2_RISE:
            break;
        case PHI2_FALL:
            if (READY->get()) {
                NEXT_STATE_FN('3', s3tmpir);
            } else {
                // STUB(" %6.2f μs: continue TW state", (TAU * 0.001));
                NEXT_STATE_FN('W', sWfetch);
            }
            break;
        }
    }

    void s3tmpir(phase_e ph)
    {
        switch (ph) {
        case UNPHASED:
            break;
        case PHI1_RISE:
            break;
        case PHI1_FALL:
            break;
        case PHI2_RISE:
            // STUB(" %6.2f μs: latch %03o from %05o into IR, release DBIN", (TAU * 0.001), Data, Addr);
            IR = Data;
            DBIN.lo();
            // TODO when do we FLOAT the Addr bus?
            break;
        case PHI2_FALL:
            // STUB(" %6.2f μs: opcode 0%03o selects T4 '%s'", (TAU * 0.001), IR, s4[IR].fn);
            NEXT_STATE_PFN('4', s4[IR]);
            break;
        }
    }

#define sss (IR & 7)
#define ddd ((IR >> 3) & 7)

    void s4ill(phase_e ph)
    {
        (void)ph;
        STUB(" illegal instruction 0%03o", IR);
        abort();
    }

    void s4nop(phase_e ph)
    {
        switch (ph) {
        case UNPHASED:
            break;
        case PHI1_RISE:
            break;
        case PHI1_FALL:
            break;
        case PHI2_RISE:
            // STUB(" %6.2f μs: execute NOP instruction", (TAU * 0.001));
            break;
        case PHI2_FALL:
            NEXT_STATE_FN('1', s1fetch);
            break;
        }
    }

    void s4movRR(phase_e ph)
    {
        switch (ph) {
        case UNPHASED:
            break;
        case PHI1_RISE:
            break;
        case PHI1_FALL:
            break;
        case PHI2_RISE:
            assert(sss < 6);
            TMP = r[6 - sss];
            break;
        case PHI2_FALL:
            NEXT_STATE_FN('5', s_movR5);
            break;
        }
    }

    void s4movRA(phase_e ph)
    {
        switch (ph) {
        case UNPHASED:
            break;
        case PHI1_RISE:
            break;
        case PHI1_FALL:
            break;
        case PHI2_RISE:
            assert(sss == 7);
            TMP = ACC;
            break;
        case PHI2_FALL:
            NEXT_STATE_FN('5', s_movR5);
            break;
        }
    }

    void s4movAR(phase_e ph)
    {
        switch (ph) {
        case UNPHASED:
            break;
        case PHI1_RISE:
            break;
        case PHI1_FALL:
            break;
        case PHI2_RISE:
            assert(sss < 6);
            TMP = r[6 - sss];
            break;
        case PHI2_FALL:
            NEXT_STATE_FN('5', s_movA5);
            break;
        }
    }

    void s_movR5(phase_e ph)
    {
        switch (ph) {
        case UNPHASED:
            break;
        case PHI1_RISE:
            break;
        case PHI1_FALL:
            break;
        case PHI2_RISE:
            assert(ddd < 6);
            r[6 - ddd] = TMP;
            break;
        case PHI2_FALL:
            NEXT_STATE_FN('1', s1fetch);
            break;
        }
    }

    void s_movA5(phase_e ph)
    {
        switch (ph) {
        case UNPHASED:
            break;
        case PHI1_RISE:
            break;
        case PHI1_FALL:
            break;
        case PHI2_RISE:
            assert(ddd == 7);
            ACC = TMP;
            break;
        case PHI2_FALL:
            NEXT_STATE_FN('1', s1fetch);
            break;
        }
    }

    void s4hlt(phase_e ph)
    {
        switch (ph) {
        case UNPHASED:
            break;
        case PHI1_RISE:
            break;
        case PHI1_FALL:
            break;
        case PHI2_RISE:
            break;
        case PHI2_FALL:
            NEXT_STATE_FN('1', s1hlta);
            break;
        }
    }

    void s1hlta(phase_e ph)
    {
        switch (ph) {
        case UNPHASED:
            break;
        case PHI1_RISE:
            status = STATUS_HALTACK;
            break;
        case PHI1_FALL:
            break;
        case PHI2_RISE:
            Addr = PC; // TODO verify Address when we are halted
            Data = status;
            SYNC.hi();
            // STUB(" %6.2f μs: with Data=%03o assert SYNC", (TAU * 0.001), Data);
            break;
        case PHI2_FALL:
            NEXT_STATE_FN('2', s2hlta);
            break;
        }
    }

    void s2hlta(phase_e ph)
    {
        switch (ph) {
        case UNPHASED:
            break;
        case PHI1_RISE:
            WAIT.hi();
            break;
        case PHI1_FALL:
            break;
        case PHI2_RISE:
            // STUB(" %6.2f μs: release SYNC, float Addr and Data", (TAU * 0.001));
            SYNC.lo();
            Addr = 0177777; // TODO represent FLOATING Addr
            Data = 0377;    // TODO represent FLOATING Data
            DBIN.hi();
            break;
        case PHI2_FALL:
            // STUB(" %6.2f μs: insert tWH state", (TAU * 0.001));
            NEXT_STATE_FN('H', sWhlta);
            break;
        }
    }

    void sWhlta(phase_e)
    {
        // TODO verify RESET behavior
        // TODO implement HOLD behavior
        // TODO implement INT behavior

        // NOTE: if we do nothing here,
        // the code should just repeat this
        // state service function forever.
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
