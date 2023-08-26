#include "clk8080.hh"
#include "edge.hh"

#include <cassert>
#include <cstdio>

class Clk8080impl : public Clk8080 {
public:
    Clk8080impl(const char* name);
    virtual ~Clk8080impl();
    virtual void linked();
    virtual tau_t tick();

protected:
    unsigned scan;
    tau_t tau_next;
    unsigned tau_frac;
    void phi1a_latch();
    void ststb_latch();
    void status_logic();

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
    union {
        struct {
#pragma GCC diagnostic pop
            // bits from low to high, on little-endian simulation hosts.
            unsigned sINTA : 1; // Interrupt Acknowledge
            unsigned sWO : 1; // Write Output (active low)
            unsigned sSTACK : 1; // Stack Memory Access
            unsigned sHLTA : 1; // Halt Acknowledge
            unsigned sOUT : 1; // Output Write
            unsigned sM1 : 1; // Instruction Fetch (including Interrupt Service)
            unsigned sINP : 1; // Input Read
            unsigned sMEMR : 1; // Meory Read
        };
        Byte sBYTE;
    };

    void status_update_end_reads();
    void status_update_using_wr();
};

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
    , scan(0)
    , tau_next(TAU)
    , tau_frac(0)
{
    Edge_RISE(&PHI1A, phi1a_latch);
    Edge_FALL(&STSTB, ststb_latch);
    Edge_RISE(WR, status_update_using_wr);
    Edge_FALL(WR, status_update_using_wr);
    Edge_RISE(DBIN, status_update_end_reads);
    Edge_FALL(DBIN, status_update_end_reads);
}

void Clk8080impl::linked()
{
    assert(RESIN);
    assert(RDYIN);
    assert(DMARQ);
    assert(INTRQ);
    assert(SYNC);
    assert(DBIN);
    assert(HLDA);
    assert(D);
}

//      phase       123456789
#define BIT_LOAD  0b100000000
#define BIT_PHI1  0b110000000
#define BIT_PHI2  0b000111100
#define BIT_PHI1A 0b000011110

void Clk8080impl::phi1a_latch()
{
    RESET.set(!RESIN->get());
    READY.set(RDYIN->get());
    HOLD.set(DMARQ->get());
    INT.set(INTRQ->get());
}

void Clk8080impl::status_update_using_wr() {
    if (WO->get()) {
        MEMW.hi();
        IOW.hi();
    } else {
        MEMW.inv(!sWO && !sOUT);
        IOW.inv(sOUT);
    }
}

void Clk8080impl::status_update_end_reads() {
    MEMR.hi();
    IOR.hi();
    INTA.hi();
}

void Clk8080impl::ststb_latch()
{
    sBYTE = *D;

    status_update_using_wr();

    MEMR.inv(sMEMR && !sHLTA);
    IOR.inv(sINP);
    INTA.inv(sINTA);
}


tau_t Clk8080impl::tick()
{
    assert(TAU <= tau_next); // crash if we are called late
    if (TAU < tau_next) // do nothing if it is not time yet
        return tau_next;

    OSC.hi();

    // This state machine was constructed by implementing Figure 3-3
    // as literally as possible, then encoding each output as a series
    // of bits that can be extraced with a scan bit that scans the
    // series.

    scan >>= 1;
    if (!scan)
        scan = BIT_LOAD;

    PHI1A.set(scan & BIT_PHI1A);
    PHI1.set(scan & BIT_PHI1);
    PHI2.set(scan & BIT_PHI2);

    // if RESET is set, pull STSTB low.
    // If SYNC and PHI1A are both high, pull STSTB low.
    STSTB.inv(RESET.get() || (SYNC->get() & PHI1A.get()));

    OSC.lo();

    // Update UNIT and compute the TAU value for our next call.
    ++UNIT;
    tau_next = TAU + 55;
    tau_frac += 5;
    if (tau_frac >= 9) {
        tau_next++;
        tau_frac -= 9;
    }
    return tau_next;
}

Clk8080impl::~Clk8080impl() { }

Clk8080* Clk8080::create(const char* name)
{
    return new Clk8080impl(name);
}

Clk8080::Clk8080(const char* name)
    : Module(name)
    , RESET(1)
{
}

Clk8080::~Clk8080() { }

// === === === === === === === === === === === === === === === ===
//                         TESTING SUPPORT
// === === === === === === === === === === === === === === === ===

#include "verify_elapsed_time.hh"

#include <iostream>

// to print a value in binary:
// #include <iostream>
// #include <bitset>
//    std::cout << "cycle: BIT_PHI1 = "
//              << std::bitset<9>(BIT_PHI1) << "\n";

// TODO: think about how I want to trace signals
// in general. This code works neatly for testing
// the clock generator, do we want something more
// general for everything?

#include "tau.hh"

#include "traced.hh"

// This is just enough logic to generate a SYNC pulse
// that looks like it came from the 8080.

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

class Sync8080 : Module {
public:
    Sync8080(const char* name)
        : Module(name)
        , next_state(0)
    {
        D = STATUS_FETCH;
        SYNC.lo();
    }

    Edge* PHI2;
    Edge* RESET;
    Edge* READY;
    Edge SYNC;

    Byte D;

    virtual void linked()
    {
        // code here is executed after our non-owned Edges
        // have been pointed at their destinations.

        Edge_RISE(PHI2, phi2_rise);
        Edge_RISE(RESET, reset_rise);
        Edge_FALL(READY, ready_fall);
    }

    int next_state;

    void reset_rise()
    {
        SYNC.lo();
        next_state = 0;
    }

    void ready_fall()
    {
        SYNC.lo();
        next_state = 0;
    }

    void phi2_rise()
    {
        if (next_state > 0) {
            SYNC.lo();
            next_state++;
            if (next_state > 3)
                next_state = 0;
        } else if (!RESET->get()) {
            SYNC.hi();
            next_state = 1;
        }
    }
};

// Timing Requirements from 8080A
// assuming an 18.00 MHz xtal

#include "verify_elapsed_time.hh"

static VerifyElapsedTime p1("P₁", 480, 2000); // Φ₁↑ to Φ₁↑
static VerifyElapsedTime p2("P₂", 480, 2000); // Φ₁↓ to Φ₁↓
static VerifyElapsedTime p3("P₃", 480, 2000); // Φ₂↑ to Φ₂↑
static VerifyElapsedTime p4("P₄", 480, 2000); // Φ₂↓ to Φ₂↓

static VerifyElapsedTime tPhi1("tΦ₁", 60, 500); // Φ₁↑ to Φ₁↓
static VerifyElapsedTime tPhi2("tΦ₂", 220, 500); // φ₂↑ to Φ₂↓
static VerifyElapsedTime tD1("tD1", 0, 500); // Φ₁↓ to Φ₂↑
static VerifyElapsedTime tD2("tD2", 70, 500); // Φ₂↓ to Φ₁↑
static VerifyElapsedTime tD3("tD3", 80, 500); // Φ₁↑ to Φ₂↑

// Check the timing of the READY signal.
// 8080 timing says
//   READY stable t_RS before Phi2 falling edge
//     120ns <= t_RS

static VerifyElapsedTime tRS("tRS", 90, 500); // READY↕ to Φ₂↓
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

void Clk8080::bist()
{
    // Create signals owned by the Outside World

    Edge RESIN; // active low
    Edge RDYIN;
    Edge DMARQ;
    Edge INTRQ;

    tau_t stop;

    // Instantiate a Clock Generator and part of a CPU.

    Clk8080& clk(*Clk8080::create("clk1"));
    Sync8080 cpu("cpu1");

    // Connect up the signals each of them imports.

    clk.RESIN = &RESIN;
    clk.RDYIN = &RDYIN;
    clk.DMARQ = &DMARQ;
    clk.INTRQ = &INTRQ;
    clk.SYNC = &cpu.SYNC;
    clk.D = &cpu.D;

    cpu.PHI2 = &clk.PHI2;
    cpu.RESET = &clk.RESET;
    cpu.READY = &clk.READY;

    Edge_RISE(&clk.PHI1, phi1_rise);
    Edge_FALL(&clk.PHI1, phi1_fall);
    Edge_RISE(&clk.PHI2, phi2_rise);
    Edge_FALL(&clk.PHI2, phi2_fall);

    Edge_RISE(&clk.READY, ready_change);
    Edge_FALL(&clk.READY, ready_change);
    Edge_RISE(&clk.HOLD, hold_change);
    Edge_FALL(&clk.HOLD, hold_change);
    Edge_RISE(&clk.INT, int_change);
    Edge_FALL(&clk.INT, int_change);

    // Tell the modules we've set up the edges.

    clk.linked();
    cpu.linked();

    std::vector<Traced*> traces;

    std::vector<tau_t> page_breaks;

    // Hmmm. Idea. break pages at the rising PHI1 edge
    // that happens before SYNC sync goes up.

    tau_t last_rising_phi1 = 0;
    clk.PHI1.rise_cb([&] { last_rising_phi1 = UNIT; });
    cpu.SYNC.rise_cb([&] { page_breaks.push_back(last_rising_phi1); });

    // traces.push_back(new Traced("OSC", clk.OSC));
    traces.push_back(new Traced("PHI1", clk.PHI1));
    traces.push_back(new Traced("PHI2", clk.PHI2));
    traces.push_back(new Traced("PHI1A", clk.PHI1A));

    traces.push_back(new Traced("RESIN", RESIN, true));
    traces.push_back(new Traced("RESET", clk.RESET));

    traces.push_back(new Traced("RDYIN", RDYIN));
    traces.push_back(new Traced("READY", clk.READY));

    traces.push_back(new Traced("DMARQ", DMARQ));
    traces.push_back(new Traced("HOLD", clk.HOLD));

    traces.push_back(new Traced("INTRQ", INTRQ));
    traces.push_back(new Traced("INT", clk.INT));

    traces.push_back(new Traced("SYNC", cpu.SYNC));
    traces.push_back(new Traced("STSTB", clk.STSTB, true));

    RESIN.lo();
    RDYIN.lo();
    stop = UNIT + 54;
    while (UNIT < stop)
        TAU = clk.tick();

    // Check that RESET is latched at the right time
    // relative to PHI2, and we get our first SYNC
    // pulse in the next state (this is not quite
    // right per 8080 but my runt sync8080 above
    // does not have the extra gap state before
    // it hits SYNC).

    RESIN.hi();
    stop = UNIT + 29;
    while (UNIT < stop)
        TAU = clk.tick();

    // Check the timing of the READY signal.
    // 8080 timing says
    //   READY stable t_RS before Phi2 falling edge
    //     90ns <= t_RS

    RDYIN.hi();
    stop = UNIT + 36;
    while (UNIT < stop)
        TAU = clk.tick();

    // Check the timing of the HOLD signal.
    // 8080 timing says
    //   HOLD stable t_HS before Phi2 falling edge
    //     120ns <= t_HS

    DMARQ.hi();
    stop = UNIT + 36;
    while (UNIT < stop)
        TAU = clk.tick();

    DMARQ.lo();
    stop = UNIT + 36;
    while (UNIT < stop)
        TAU = clk.tick();

    // Check the timing of the INT signal
    // 8080 timing says
    //   INT stable t_IS before Phi2 falling edge
    //     100ns <= t_IS

    INTRQ.hi();
    stop = UNIT + 36;
    while (UNIT < stop)
        TAU = clk.tick();

    INTRQ.lo();
    stop = UNIT + 36;
    while (UNIT < stop)
        TAU = clk.tick();

    // run it out a bit.
    stop = UNIT + 52;
    while (UNIT < stop)
        TAU = clk.tick();

    tau_t u_max = UNIT;

    std::cout << std::endl;
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

    int maxm = 72;

    page_breaks.push_back(u_max);

    tau_t u = 0;
    for (auto m : page_breaks) {
        while (u < m) {
            tau_t hi = u + maxm;
            if (hi > m)
                hi = m;
            std::cout << std::endl;
            for (auto* it : traces)
                it->print(u, hi - u);
            u = hi;
        }
    }
    std::cout << std::endl;

    printf("clk_asserts passed\n");
}

// /MEMR low if WO high, MEMR high, HLTA low
// /MEMW low if WO low,  MEMR low,  OUT high

// STATUS_FETCH    0b10100010 -> /MEMR
// STATUS_MREAD    0b10000010 -> /MEMR
// STATUS_SREAD    0b10000110 -> /MEMR

// STATUS_MWRITE   0b00000000 -> /MEMW
// STATUS_SWRITE   0b00000100 -> /MEMW

// STATUS_INPUTRD  0b01000010 -> /IOR

// STATUS_OUTPUTWR 0b00010000 -> /IOW

// STATUS_HALTACK  0b10001010 -> (none)

// STATUS_INTACK   0b00100011 -> /INTA
// STATUS_INTACKW  0b00101011 -> /INTA
