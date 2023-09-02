#include "clock.h"

#include <assert.h>
#include <time.h>
#include <stdio.h>

#include "rtc.h"

#include "bist_macros.h"

static void clock_hi(Tau *rises)
{
    (*rises)++;
}

static void clock_lo(Tau *falls)
{
    (*falls)++;
}

static void Clock_bench()
{
    PRINT_TOP();

    Tau                 max_iter = 1000;
    Tau                 t0, dt;
    Tau                 mint = 25000000;

    while (1) {
        t0 = rtc_ns();
        for (Tau i = 0; i < max_iter; ++i) {
            Clock_cycle();
        }
        dt = rtc_ns() - t0;
        if (dt >= mint)
            break;
        if (dt <= mint / 10) {
            max_iter *= 10;
        } else {
            max_iter = (max_iter * mint * 2.0) / dt;
        }
    }


    fprintf(stderr, "\n");
    fprintf(stderr, "Clock benchmark:\n");
    fprintf(stderr, "  max_iter is %lu\n", max_iter);
    fprintf(stderr, "  elapsed time is %.3f ms\n", dt / 1000.0);

    double              ns_per_call = dt * 1.0 / max_iter;

    fprintf(stderr, "  time per count is %.3f ns\n", ns_per_call);
    fprintf(stderr, "\n");

    // typically at -O2 and -O3 this is 5-6 ns.
    // even at -g -O0, we should be around 12 ns,
    // fail this test if we hit 50 ns.
    assert(ns_per_call < 50.0);

    PRINT_END();
}

void Clock_bist()
{
    PRINT_TOP();

    Clock_init(1000, 18);

    TAU = 0;
    UNIT = 0;

    Tau                 rises = 0;
    EDGE_RISE(CLOCK, clock_hi, &rises);

    Tau                 falls = 0;
    EDGE_FALL(CLOCK, clock_lo, &falls);

    for (int i = 1; i <= 18; ++i) {
        Clock_cycle();
        assert(rises == i);
        assert(falls == i);
        assert(UNIT == i);
        // fprintf(stderr, "%3lu: expected %lu, observed %lu\n", UNIT, (UNIT*500)/9 , TAU);
        assert(TAU == (i * 500) / 9);
    }

    assert(TAU == 1000);

    Clock_bench();

    PRINT_END();
}
