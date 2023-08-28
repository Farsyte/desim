#include "clk8080.hh"

#include "timer.hh"
#include "traced.hh"
#include "verify_elapsed_time.hh"

#include "8080_status.hh"
//#include "util.hh"

#include <cassert>
#include <cstdio>
//#include <iostream>

// to print a value in binary:
// #include <iostream>
// #include <bitset>
//    std::cout << "cycle: BIT_PHI1 = "
//              << std::bitset<9>(BIT_PHI1) << "\n";

//#include "tau.hh"

// This is just enough logic to generate a SYNC pulse
// that looks like it came from the 8080.

// Timing Requirements from 8080A
// assuming an 18.00 MHz xtal

static VerifyElapsedTime p1("P₁", 480, 2000); // Φ₁↑ to Φ₁↑
static VerifyElapsedTime p2("P₂", 480, 2000); // Φ₁↓ to Φ₁↓
static VerifyElapsedTime p3("P₃", 480, 2000); // Φ₂↑ to Φ₂↑
static VerifyElapsedTime p4("P₄", 480, 2000); // Φ₂↓ to Φ₂↓

static VerifyElapsedTime tPhi1("tΦ₁", 60, 500);  // Φ₁↑ to Φ₁↓
static VerifyElapsedTime tPhi2("tΦ₂", 220, 500); // φ₂↑ to Φ₂↓
static VerifyElapsedTime tD1("tD1", 0, 500);     // Φ₁↓ to Φ₂↑
static VerifyElapsedTime tD2("tD2", 70, 500);    // Φ₂↓ to Φ₁↑
static VerifyElapsedTime tD3("tD3", 80, 500);    // Φ₁↑ to Φ₂↑

// Check the timing of the READY signal.
// 8080 timing says
//   READY stable t_RS before Phi2 falling edge
//     120ns <= t_RS

static VerifyElapsedTime tRS("tRS", 90, 500);  // READY↕ to Φ₂↓
static VerifyElapsedTime tHS("tHS", 120, 500); // HOLD↕ to Φ₂↓
static VerifyElapsedTime tIS("tIS", 100, 500); // INT↕ to Φ₂↓

static void phi1_rise()
{
    double now = TAU;
    p1.record(now);
    p1.start(now);
    tD2.record(now);
    tD3.start(now);
    tPhi1.start(now);
}

static void phi1_fall()
{
    double now = TAU;
    p2.record(now);
    p2.start(now);
    tPhi1.record(now);
    tD1.start(now);
}

static void phi2_rise()
{
    double now = TAU;
    p3.record(now);
    p3.start(now);
    tD1.record(now);
    tD3.record(now);
    tPhi2.start(now);
}

static void phi2_fall()
{
    double now = TAU;
    p4.record(now);
    p4.start(now);
    tPhi2.record(now);
    tD2.start(now);
    tRS.record(now);
    tHS.record(now);
    tIS.record(now);
}

static void ready_change()
{
    double now = TAU;
    tRS.start(now);
}

static void hold_change()
{
    double now = TAU;
    tHS.start(now);
}

static void int_change()
{
    double now = TAU;
    tIS.start(now);
}

class RecentHistory {
public:
    RecentHistory(const char* name, Edge& edge)
        : name(name)
        , edge(edge)
        , last_rise(0)
        , last_fall(0)
    {
        edge.rise_cb([=] { last_rise = TAU; edges ++; });
        edge.fall_cb([=] { last_fall = TAU; edges ++; });
    }

    const char* name;
    Edge&       edge;
    unsigned    edges;
    tau_t       last_rise;
    tau_t       last_fall;
};

static void benchmark(Action cycle)
{
    static unsigned max_iter = 1000;
    unsigned long   us_total;

    while (true) {

        unsigned togo = max_iter;

        Timer t;

        TAU  = 0;
        UNIT = 0;

        t.tick();
        while (togo-- > 0)
            cycle();
        t.tock();

        us_total = t.microseconds();
        if (us_total > 100000)
            break;
        if (us_total < 100)
            max_iter *= 10;
        else
            max_iter = (max_iter * 200000) / us_total;
    }

    fprintf(stderr, "wall clock time: %10.3f ms\n", us_total / 1000.0);
    fprintf(stderr, "simulation time: %10.3f ms\n", TAU / 1000000.0);

    //    fprintf(stderr, "Simulation of %.1f ms required %.3f ms wall time.\n",
    //            TAU / 1000000.0, us_total / 1000.0);

    fprintf(stderr, "Clock: %g ns (wall) per ms of simulation\n",
        us_total * 1000000.0 / TAU);
}

void Clk8080::bist()
{
    TAU  = 0;
    UNIT = 0;

    // The master oscillator comes from "the world"

    Edge OSC;

    Action tick = [&] {
        const double  MHz  = 18.00;
        static double pend = 0.00;

        OSC.hi();
        OSC.lo();

        pend += 1000.0 / MHz;
        TAU += (int)pend;
        pend -= (int)pend;
        UNIT++;
    };

    std::function<void(tau_t)> tick_to = [&](tau_t end_tau) {
        while (TAU < end_tau)
            tick();
    };

    std::function<void(double)> tick_us = [&](double us) {
        tick_to(TAU + (int)(us * 1000));
    };

    // Instantiate a Clock Generator and part of a CPU.

    Clk8080& clk(*Clk8080::create("clk1"));

    // make local versions of the signals the clock emits.
    Edge& PHI1(clk.PHI1);
    Edge& PHI2(clk.PHI2);
    Edge& PHI1A(clk.PHI1A);
    Edge& RESET(clk.RESET);
    Edge& READY(clk.READY);
    Edge& HOLD(clk.HOLD);
    Edge& INT(clk.INT);
    Edge& STSTB(clk.STSTB);
    Edge& MEMR(clk.MEMR);
    Edge& MEMW(clk.MEMW);
    Edge& IOR(clk.IOR);
    Edge& IOW(clk.IOW);
    Edge& INTA(clk.INTA);

    // Create the edges that the clock generator imports;
    // we will need to wiggle them at appropriate times.

    clk.OSC = &OSC;

    // Initial state is "at power on reset"

    Edge RESIN;
    Edge RDYIN;
    Edge DMARQ;
    Edge INTRQ;
    Edge SYNC;

    clk.RESIN = &RESIN;
    clk.RDYIN = &RDYIN;
    clk.DMARQ = &DMARQ;
    clk.INTRQ = &INTRQ;
    clk.SYNC  = &SYNC;

    Byte D;        // input: CPU Data Bus
    Edge DBIN;     // input: DBIN from 8080
    Edge WR { 1 }; // input: /WR from 8080
    Edge HLDA;     // input: HLDA from 8080

    assert(WR.get());

    clk.D    = &D;
    clk.DBIN = &DBIN;
    clk.WR   = &WR;
    clk.HLDA = &HLDA;

    // Tell the module we've set up the edges.

    clk.linked();

    // State numbers used here to simulate the CPU.
    // - state changes on PHI1 rising edge
    // - first state of machine cycle is 1 (for T1)
    // - normally at PHI1 rising edge, state increments
    // - if state was state_last, moves to T1
    // - on entry to T3, state_last_next is copied into state_last.
    // this allows the test code to set "state_last_next" when our
    // system is just before PHI1 rising in T1, and still get into
    // T1 of the next cycle.
    unsigned state           = 0;
    unsigned state_last      = 4;
    unsigned state_last_next = 4;
    tau_t    state_1_tau     = 0;
    PHI1.rise_cb([&] {
        if (++state > state_last)
            state = 1;
        if (state == 1)
            state_1_tau = TAU;
        if (state == 3)
            state_last = state_last_next;
    });

    Action tick_to_end_of_machine_cycle = [&] {
        // if we are in the last state of a machine cycle,
        // run precisely one tick as the common case is that
        // we are called before the tick that will move us
        // into the T1 of the next machine cycle.
        if (state == state_last)
            tick();
        while (state != state_last)
            tick();
        tick_to(state_1_tau + 500 * state_last);
    };

    // page_title = "power-on reset";
    // Record a bit of time in power-on state.
    tick_us(2.0);
    assert(clk.RESET.get());
    // manual pagebreak until SYNC pulses are running
    // page_breaks.push_back(PageBreak(UNIT, page_title));

    tick_us(1.0);
    assert(clk.RESET.get());

    // Release /RESIN, to verify RESET timing.
    RESIN.hi();
    // page_title = "coming out of reset";
    tick_to_end_of_machine_cycle();
    // manual pagebreak until SYNC pulses are running
    // page_breaks.push_back(PageBreak(UNIT, page_title));

    // benchmark before adding the rest of the instrumentation.

    benchmark(tick_to_end_of_machine_cycle);

    UNIT = 0;
    TAU  = 0;

    // Attach observers to the clock generator outputs.

    Edge_RISE(&PHI1, phi1_rise);
    Edge_FALL(&PHI1, phi1_fall);
    Edge_RISE(&PHI2, phi2_rise);
    Edge_FALL(&PHI2, phi2_fall);

    Edge_RISE(&READY, ready_change);
    Edge_FALL(&READY, ready_change);
    Edge_RISE(&HOLD, hold_change);
    Edge_FALL(&HOLD, hold_change);
    Edge_RISE(&INT, int_change);
    Edge_FALL(&INT, int_change);

    std::vector<Traced*> traces;

    struct PageBreak {
        PageBreak(tau_t t, const char* s)
            : t(t)
            , s(s)
        {
        }
        tau_t       t;
        const char* s;
    };

    std::vector<PageBreak> page_breaks;
    const char*            page_title      = "";
    tau_t                  phi1rise_unit   = 0;
    const char*            phi1rise_title1 = "";
    const char*            phi1rise_title2 = "";

    PHI1.rise_cb([&] {
        phi1rise_unit   = UNIT;
        phi1rise_title2 = phi1rise_title1;
        phi1rise_title1 = page_title;
    });

    Action that_phi1_rise_is_pagebreak = [&] {
        page_breaks.push_back(PageBreak(phi1rise_unit, phi1rise_title2));
    };
    SYNC.rise_cb(that_phi1_rise_is_pagebreak);

    // traces.push_back(new Traced("OSC", OSC));

    traces.push_back(new Traced("PHI1", PHI1));
    traces.push_back(new Traced("PHI2", PHI2));
    traces.push_back(new Traced("PHI1A", PHI1A));

    traces.push_back(new Traced("RESIN", RESIN, true));
    traces.push_back(new Traced("RESET", RESET));

    traces.push_back(new Traced("SYNC", SYNC));
    traces.push_back(new Traced("STSTB", STSTB, true));

    traces.push_back(new Traced("RDYIN", RDYIN, true));
    traces.push_back(new Traced("READY", READY, true));

    traces.push_back(new Traced("DMARQ", DMARQ));
    traces.push_back(new Traced("HOLD", HOLD));

    traces.push_back(new Traced("INTRQ", INTRQ));
    traces.push_back(new Traced("INT", INT));

    traces.push_back(new Traced("DBIN", DBIN));
    traces.push_back(new Traced("WR", WR, true));
    traces.push_back(new Traced("HLDA", HLDA));

    traces.push_back(new Traced("MEMR", MEMR, true));
    traces.push_back(new Traced("MEMW", MEMW, true));
    traces.push_back(new Traced("IOR", IOR, true));
    traces.push_back(new Traced("IOW", IOW, true));
    traces.push_back(new Traced("INTA", INTA, true));

    // Add test logic to drive SYNC, and place status on D,
    // during the appropriate time. Any changes to 'status'
    // will not take effect until the next PHI2 rising edge.
    unsigned status = STATUS_RESET;

    PHI2.rise_cb([&] {
        switch (state) {
        case 1:
            D = status;
            SYNC.hi();
            break;
        case 2:
            D = 0xFF;
            SYNC.lo();
            break;
        }
    });

    // run a couple of machine cycles just with
    // the bare SYNC logic.

    page_title = "initial operation";
    tick_us(4.0);

    // check timing of RDYIN->READY

    page_title = "check RDYIN ==> READY timing";
    tick_us(1.0);
    RDYIN.hi();
    tick_us(1.0);

    // check timing of DMARQ->HOLD

    page_title = "check /DMARQ ==> /HOLD timing";
    DMARQ.hi();
    tick_us(1.0);
    DMARQ.lo();
    tick_us(1.0);

    // check timing of INTRQ->INT

    page_title = "check /INTRQ ==> /INT timing";
    INTRQ.hi();
    tick_us(1.0);
    INTRQ.lo();
    tick_us(1.0);

    // Verify that CPU STATUS is decoded proprely
    // into /MEMR /MEMW /IOR /IOW /INTA
    //
    // Rather than write the simulation of the logic
    // in the 8080 that derives DBIN and WR, I will
    // just include the information in the test vector.

    status           = STATUS_FETCH;
    unsigned want_DI = 0;
    unsigned want_WR = 0;

    // modify test harness to generate DBIN and WR
    // as directed by the helper functions. No need
    // to simulate the logic that drives these when
    // we can just say what we want in the test vector.

    // add logic to drive DBIN at the right time if
    // we want DBIN asserted in this machine cycle.

    PHI2.rise_cb([&] {
        switch (state) {
        case 2:
            DBIN.set(want_DI);
            break;
        case 3:
            DBIN.lo();
            break;
        }
    });

    // add logic to drive WR at the right time if
    // we want WR asserted in this machine cycle.

    PHI1.rise_cb([&] {
        switch (state) {
        case 3:
            WR.inv(want_WR);
            break;
        case 1:
        case 4:
            WR.hi();
            break;
        }
    });

    const char* titles[10] = {
        "(1) STATUS_FETCH",
        "(2) STATUS_MREAD",
        "(3) STATUS_MWRITE",
        "(4) STATUS_SREAD",
        "(5) STATUS_SWRITE",
        "(6) STATUS_INPUTRD",
        "(7) STATUS_OUTPUTWR",
        "(8) STATUS_INTACK",
        "(9) STATUS_HALTACK",
        "(10) STATUS_INTACKW",
    };

    unsigned stv[10][5] = {
        // DI WR ML CT
        { 1, 0, 4, 0, STATUS_FETCH },    // "(1) STATUS_FETCH",
        { 1, 0, 3, 0, STATUS_MREAD },    // "(2) STATUS_MREAD",
        { 0, 1, 3, 0, STATUS_MWRITE },   // "(3) STATUS_MWRITE",
        { 1, 0, 3, 0, STATUS_SREAD },    // "(4) STATUS_SREAD",
        { 0, 1, 3, 0, STATUS_SWRITE },   // "(5) STATUS_SWRITE",
        { 1, 0, 3, 0, STATUS_INPUTRD },  // "(6) STATUS_INPUTRD",
        { 0, 1, 3, 0, STATUS_OUTPUTWR }, // "(7) STATUS_OUTPUTWR",
        { 1, 0, 5, 0, STATUS_INTACK },   // "(8) STATUS_INTACK",
        { 0, 0, 3, 0, STATUS_HALTACK },  // "(9) STATUS_HALTACK",
        { 1, 0, 3, 0, STATUS_INTACKW },  // "(10) STATUS_INTACKW",
    };

    // start tracking recent history of the control signals.
    RecentHistory rh_MEMR("MEMR", MEMR);
    RecentHistory rh_MEMW("MEMW", MEMW);
    RecentHistory rh_IOR("IOR", IOR);
    RecentHistory rh_IOW("IOW", IOW);
    RecentHistory rh_INTA("INTA", INTA);

    for (int tvi = 0; tvi < 10; ++tvi) {
        unsigned* tv     = stv[tvi];
        want_DI          = *tv++; // for PHI2 rising edge response above
        want_WR          = *tv++; // for PHI1 rising edge response above
        unsigned want_ML = *tv++;
        unsigned want_CT = *tv++;
        status           = *tv++;
        page_title       = titles[tvi];
        // set up state_last during T2.
        // NOTE: if we set it before T1,
        // then we would only run T4 and T5 of INTACK.
        while (state != 2)
            tick();
        state_last = want_ML;

        tau_t cstart = TAU;
        tick_to_end_of_machine_cycle();

        if (want_CT == 1) {
            assert(cstart < rh_MEMR.last_fall);
            assert(rh_MEMR.last_fall < rh_MEMR.last_rise);
        } else {
            assert(rh_MEMR.last_fall <= cstart);
        }

        if (want_CT == 2) {
            assert(cstart < rh_MEMW.last_fall);
            assert(rh_MEMW.last_fall < rh_MEMW.last_rise);
        } else {
            // MEMW may not deassert until PHI1 rises
            // assert(rh_MEMW.last_fall <= cstart);
        }

        if (want_CT == 3) {
            assert(cstart < rh_IOR.last_fall);
            assert(rh_IOR.last_fall < rh_IOR.last_rise);
        } else {
            assert(rh_IOR.last_fall <= cstart);
        }

        if (want_CT == 4) {
            assert(cstart < rh_IOW.last_fall);
            assert(rh_IOW.last_fall < rh_IOW.last_rise);
        } else {
            // IOW may not deassert until PHI1 rises
            // assert(rh_IOW.last_fall <= cstart);
        }

        if (want_CT == 5) {
            assert(cstart < rh_INTA.last_fall);
            assert(rh_INTA.last_fall < rh_INTA.last_rise);
        } else {
            assert(rh_INTA.last_fall <= cstart);
        }
    }

    tau_t u_max = UNIT;

    printf("\n");
    p1.print();
    p2.print();
    p3.print();
    p4.print();
    tPhi1.print();
    tPhi2.print();
    tD1.print();
    tD2.print();
    tD3.print();
    tRS.print();
    tHS.print();
    tIS.print();

    for (auto* it : traces)
        it->update_trace();

    // Tune "maxm" to limit the width of data plots.
    int maxm = 72;

    page_breaks.push_back(PageBreak(u_max, page_title));

    double us_per_unit = (TAU * 0.001) / UNIT;

    tau_t u = 0;
    for (auto m : page_breaks) {
        while (u < m.t) {
            tau_t hi = u + maxm;
            if (hi > m.t)
                hi = m.t;
            printf("\n");
            printf("From %.3f to %.3f μs: %s\n", us_per_unit * u, us_per_unit * hi, m.s);
            for (auto* it : traces)
                it->print(u, hi - u);
            u = hi;
        }
    }
    printf("\n");

    printf("clk_asserts passed\n");
}
