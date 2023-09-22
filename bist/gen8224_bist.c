#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "bist_macros.h"
#include "clock.h"
#include "gen8224.h"
#include "rtc.h"
#include "stub.h"
#include "timing_check.h"
#include "traced.h"

static Edge         RESIN_ = { {"RESIN_"} };
static Edge         RDYIN = { {"RDYIN"} };
static Edge         SYNC = { {"SYNC"} };

static Gen8224      gen = { {"gen1", CLOCK, RESIN_, RDYIN, SYNC} };

static Traced       tRESIN_ = { {"tRESIN_"} };
static Traced       tRDYIN = { {"tRDYIN"} };
static Traced       tSYNC = { {"tSYNC"} };
static Traced       tPHI1 = { {"tPHI1"} };
static Traced       tPHI2 = { {"tPHI2"} };
static Traced       tRESET = { {"tRESET"} };
static Traced       tREADY = { {"tREADY"} };
static Traced       tSTSTB_ = { {"tSTSTB_"} };

static pTraced      trace_list[] = {
    tPHI1,
    tPHI2,
    tSYNC,
    tSTSTB_,
    tRESIN_,
    tRESET,
    tRDYIN,
    tREADY,
};
static size_t       trace_count = sizeof trace_list / sizeof trace_list[0];

// Timing for 8224 for t_CY=500ns, f=18.00 MHz:
//
//  tcy/9 =  55.55~
// 2tcy/9 = 111.11~
// 4tcy/9 = 222.22~
// 5tcy/9 = 277.77~
// 6tcy/9 = 333.33~
//
// t_Φ₁:  Φ₁↑ to Φ₁↓ is at least 91.11 ns                       [2tcy/9-20]
// t_Φ₂:  Φ₂↑ to Φ₂↓ is at least 242.77 ns                      [5tcy/9-35]
//
// t_D1:  Φ₁↓ to Φ₂↑ is at least 0 ns
// t_D2:  Φ₂↓ to Φ₁↑ is at least 97.11 ns                       [2tcy/9-14]
// t_D3:  Φ₁↑ to Φ₂↑ is 111.11 to 131.11 ns                     [2tcy/9,+20]
//
// t_DSS: Φ₂ to /STSTB is 303.33 to 333.33 ns                   [6tcy/9-30,6tcy]
// t_PW:  /STSTB↓ to ↑ is at least 25.55 ns                     [1tcy/9-30]
// t_DRH: /STSTB↓ to READY/RESET↕ at least 222.22 ns            [4tcy/9]
// t_DR:  READY/RESET↕ to Φ₂↓ is at least 197.22 ns             [4tcy/9-25]
//
// Timing Requirements from 8080A
//
// t_CY:  Φ₁↑ to Φ₁↑ is 0.48 to 2.0 μs
// t_Φ₁:  Φ₁↑ to Φ₁↓ is at least 60 ns
// t_Φ₂:  Φ₂↑ to Φ₂↓ is at least 60 ns
//
// t_D1: Φ₁↓ to Φ₂↑ is at least 0 ns
// t_D2: Φ₂↓ to Φ₁↑ is at least 70 ns
// t_D3: Φ₁↑ to Φ₂↑ is at least 70 ns
//
// t_RS: READY↕ to Φ2↓ is at least 120 ns
//
// TODO: check that Φ₁ and Φ₂ are not both high at the same time.
// TODO: timing of RESET signal
// TODO: timing of /STSTB signal

static TimingCheck  tCY = { {"CY", 480, 2000} };

static TimingCheck  tPhi1 = { {"Φ₁", 91, 0} };       // 8080 needs at least 60 ns
static TimingCheck  tPhi2 = { {"Φ₂", 242, 0} };      // 8080 needs at least 220 ns

static TimingCheck  tD1 = { {"D1", 0, 0} };
static TimingCheck  tD2 = { {"D2", 97, 0} };    // 8080 needs at least 70 ns
static TimingCheck  tD3 = { {"D3", 111, 132} }; // 8080 needs at least 80 ns

static TimingCheck  tDSS = { {"DSS", 303, 334} };
static TimingCheck  tPW = { {"PW", 25, 0} };
static TimingCheck  tDRH = { {"DRH", 217, 0} };
static TimingCheck  tDR = { {"DR", 197, 0} };

static TimingCheck  tRS = { {"RS", 120, 0} };   // this is the 8080 requirement

static void phi1_rise(void *arg)
{
    (void)arg;
    timing_check_record(tCY);
    timing_check_start(tCY);
    timing_check_record(tD2);
    timing_check_start(tD3);
    timing_check_start(tPhi1);
}

static void phi1_fall(void *arg)
{
    (void)arg;
    timing_check_record(tPhi1);
    timing_check_start(tD1);
}

static void phi2_rise(void *arg)
{
    (void)arg;
    timing_check_record(tD1);
    timing_check_record(tD3);
    timing_check_start(tPhi2);
    timing_check_start(tDSS);
}

static void phi2_fall(void *arg)
{
    (void)arg;
    timing_check_record(tDR);
    timing_check_record(tPhi2);
    timing_check_start(tD2);
    timing_check_record(tRS);
}

static void ready_change(void *arg)
{
    (void)arg;
    timing_check_start(tRS);
    timing_check_record(tDRH);
    timing_check_start(tDR);
}

static void reset_change(void *arg)
{
    (void)arg;
    timing_check_start(tRS);
    timing_check_record(tDRH);
    timing_check_start(tDR);
}

static void ststb_fall(void *arg)
{
    (void)arg;
    timing_check_record(tDSS);
    timing_check_start(tPW);
    timing_check_start(tDRH);
}

static void ststb_rise(void *arg)
{
    (void)arg;
    timing_check_record(tPW);
}

void Gen8224_bench()
{
    PRINT_TOP();

    Edge_lo(SYNC);      /* start with SYNC disabled (required initial condition) */
    Edge_lo(RESIN_);    /* start with RESIN asserted */
    Edge_lo(RDYIN);     /* start with RDYIN not asserted */

    Clock_init(18.00);
    assert(TAU == 0);
    assert(UNIT == 0);
    Gen8224_init(gen);
    Gen8224_linked(gen);

    Edge_hi(SYNC);      /* mostly test with SYNC always asserted */

    Tau                 delta_unit = 1000;
    Tau                 wall_t0_ns, wall_dt_ns;
    double              sim_t0_us = 0;
    double              sim_dt_us = 0;
    Tau                 mint = 25000000;

    while (1) {

        sim_t0_us = TU;
        wall_t0_ns = rtc_ns();
        Clock_cycle_by(delta_unit);
        wall_dt_ns = rtc_ns() - wall_t0_ns;
        sim_dt_us = TU - sim_t0_us;

        if (wall_dt_ns >= mint)
            break;
        if (wall_dt_ns <= mint / 10) {
            delta_unit *= 10;
        } else {
            delta_unit = (delta_unit * mint * 2.0) / wall_dt_ns;
        }
    }

    double              w_ms = wall_dt_ns / 1000000.0;
    double              s_ms = sim_dt_us / 1000.0;

    fprintf(stderr, "\n");
    fprintf(stderr, "Gen8224 benchmark:\n");
    fprintf(stderr, "  wall time elapsed: %.3f ms\n", w_ms);
    fprintf(stderr, "   sim time elapsed: %.3f ms\n", s_ms);

    double              time_ratio = s_ms / w_ms;
    fprintf(stderr, "  sim running at %.2fx real time" " (higher is better)\n", time_ratio);
    fprintf(stderr, "\n");

    // I am simulating the 8224 at several times its original
    // speed, configured to use a 18.00 MHz clock.
    //
    // Compiled with -g -O0, sim runs at about 4x real time.
    // Compiled with -g -O2, sim runs at about 7x real time.
    // Compiled with -g -O3, sim runs at about 9x real time.
    //
    // Compiled with -g -pg -O0, sim runs at about 1x real time.
    //
    // Fail this test if the ratio falls under 0.5
    //
    assert(time_ratio > 0.5);

    PRINT_END();
}

void Gen8224_bist()
{
    PRINT_TOP();

    // complete reset.
    // Note that the init routines blow away
    // the subscriber lists.
    Clock_init(18.00);
    assert(TU == 0);
    assert(UNIT == 0);
    Gen8224_init(gen);
    Gen8224_linked(gen);

    Edge_hi(SYNC);      /* no cpu, mostly test with SYNC always asserted */
    Edge_lo(RESIN_);    /* start with RESIN asserted */
    Edge_lo(RDYIN);     /* start with RDYIN not asserted */

    Traced_init(tRESIN_, gen->RESIN_, 1);
    Traced_init(tRDYIN, gen->RDYIN, 0);
    Traced_init(tSYNC, gen->SYNC, 0);

    Traced_init(tPHI1, gen->PHI1, 0);
    Traced_init(tPHI2, gen->PHI2, 0);

    Traced_init(tRESET, gen->RESET, 0);
    Traced_init(tREADY, gen->READY, 0);
    Traced_init(tSTSTB_, gen->STSTB_, 1);

    Traced_active_boring(tSYNC);
    Traced_active_boring(tRDYIN);
    Traced_active_boring(tREADY);

    Tau                 umin = UNIT;

    // Run for a while with reset and ready both low
    Clock_cycle_by(4 * 9);

    /// Now start capturing information about the signals.

    EDGE_RISE(gen->PHI1, phi1_rise, "");
    EDGE_FALL(gen->PHI1, phi1_fall, "");
    EDGE_RISE(gen->PHI2, phi2_rise, "");
    EDGE_FALL(gen->PHI2, phi2_fall, "");

    EDGE_RISE(gen->READY, ready_change, "");
    EDGE_FALL(gen->READY, ready_change, "");
    EDGE_RISE(gen->RESET, reset_change, "");
    EDGE_FALL(gen->RESET, reset_change, "");
    EDGE_FALL(gen->STSTB_, ststb_fall, "");
    EDGE_RISE(gen->STSTB_, ststb_rise, "");

    // Run for a while with reset and ready both low
    Clock_cycle_by(4 * 9);

    for (int outer = 0; outer < 18; ++outer) {
        Edge_set(RESIN_, !Edge_get(RESIN_));
        Clock_cycle_by(2 * 9);
        Edge_set(RESIN_, !Edge_get(RESIN_));
        Clock_cycle_by(2 * 9);
        Clock_cycle();  // slip us one inner phase
    }

    Edge_hi(RESIN_);
    Clock_cycle_by(4 * 9);

    for (int outer = 0; outer < 18; ++outer) {
        Edge_set(RDYIN, !Edge_get(RDYIN));
        Clock_cycle_by(2 * 9);
        Edge_set(RDYIN, !Edge_get(RDYIN));
        Clock_cycle_by(2 * 9);
        Clock_cycle();  // slip us one inner phase
    }

    Edge_hi(RDYIN);
    Clock_cycle_by(4 * 9);

    // Testing without a CPU, just hold SYNC high.
    Edge_hi(SYNC);
    Clock_cycle_by(20 * 9);

    printf("\n");
    printf("Timing Checks:\n");
    timing_check_print(tCY);
    timing_check_print(tPhi1);
    timing_check_print(tPhi2);
    timing_check_print(tD1);
    timing_check_print(tD2);
    timing_check_print(tD3);
    timing_check_print(tDSS);
    timing_check_print(tPW);
    timing_check_print(tDRH);
    timing_check_print(tDR);
    timing_check_print(tRS);

    printf("\n");
    printf("Signal Traces:\n");
    for (size_t i = 0; i < trace_count; ++i)
        Traced_update(trace_list[i]);

    double              us_per_unit = TU / UNIT;

    Tau                 maxm = 72;
    Tau                 u = umin;
    Tau                 umax = UNIT;
    while (u < umax) {
        Tau                 hi = u + maxm;
        if (hi > umax)
            hi = umax;
        printf("\n");
        printf("From %.3f to %.3f μs:\n", us_per_unit * u, us_per_unit * hi);
        for (size_t i = 0; i < trace_count; ++i)
            Traced_print(trace_list[i], u, hi - u);
        u = hi;
    }
    PRINT_END();
}
