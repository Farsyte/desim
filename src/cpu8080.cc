#include "cpu8080.hh"

#include <cstdio>
#include <cassert>

#include "action.hh"
#include "tau.hh"
#include "8080_status.hh"

class Cpu8080impl : public Cpu8080 {
public:
    Cpu8080impl(const char* name);
    virtual void linked();
protected: 

    char curr_state;
    char next_state;
    char last_state;

    void reset();
    void init_state_manager();
    void init_sync_manager();
    void init_dbin_manager();
    void init_wr_manager();

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
    union {
        struct {
#pragma GCC diagnostic pop
            STATUS_BIT_DECLS;
        };
        Byte status;
    };
};

Cpu8080impl::Cpu8080impl(const char* name)
    : Cpu8080(name)
{
    // Edge_RISE(&OWNED_EDGE, fn);
}

#include "link_assert.hh"
void Cpu8080impl::linked()
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

    reset();
    init_state_manager();
    init_sync_manager();
    init_dbin_manager();
    init_wr_manager();
}

void Cpu8080impl::reset() {

    curr_state = 'R';
    next_state = 'R';
    last_state = 'R';

    status = STATUS_RESET;
}

void Cpu8080impl::init_state_manager()
{
    // Transition to the next state on the rising edge
    // of the PHI1 clock. If we are in T1 through T8,
    // the next state defaults to T2 through T9. If the
    // state designation is a letter, default is to
    // remain in the current state.
    //
    // 8080 uses T1, T2, TW, T3, T4, T5.
    // desim uses TR for RESET states

    PHI1->rise_cb([=] {
        curr_state = next_state;
        if ('0' <= next_state && next_state <= '9') {
            if (curr_state > last_state) {
                curr_state = '1';
                last_state = '4';
            }
            next_state = curr_state + 1;
        }
    });

    RESET->rise_cb([=] {
        reset();
    });

    RESET->fall_cb([=] {
        next_state = '1';
        last_state = '4';
        status = STATUS_FETCH;
    });
}

void Cpu8080impl::init_sync_manager()
{
    PHI2->rise_cb([=] {
        switch (curr_state) {
        case '1':
            D = status;
            SYNC.hi();
            break;
        case '2':
            D = 0377; // TODO better way to represent floating
            SYNC.lo();
            break;
        }
    });
}

void Cpu8080impl::init_dbin_manager()
{
    PHI2->rise_cb([=] {
        switch (curr_state) {
        case '2':
            if ((sM1 || sMEMR) && !sHLTA)
                DBIN.hi();
            break;
        case '3':
            DBIN.lo();
            break;
        }
    });
}

void Cpu8080impl::init_wr_manager()
{
    PHI1->rise_cb([=] {
        switch (curr_state) {
        case 'W': /* FALLTHRU */
        case '3':
            if (!sWO)
                WR.lo();
            break;
        case '1': /* FALLTHRU */
        case '4':
            WR.hi();
            break;
        }
    });
}

Cpu8080* Cpu8080::create(const char* name)
{
    return new Cpu8080impl(name);
}

Cpu8080::Cpu8080(const char* name)
    : Module(name)
    , WR(1)
{
}
