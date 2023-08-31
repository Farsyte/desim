#include "clock.h"

#include <assert.h>
#include <time.h>
#include <stdio.h>

static void clock_hi(Tau *rises)
{
    (*rises)++;
}

static void clock_lo(Tau *falls)
{
    (*falls)++;
}

static double rtc()
{
    struct timespec     tp[1];
    clock_gettime(CLOCK_MONOTONIC_RAW, tp);
    return tp->tv_sec + tp->tv_nsec / 1000000000.0;
}

static void Clock_bench()
{
    Tau                 max_iter = 1000;
    double              t0, dt;
    double              mint = 0.25;

    while (1) {
        t0 = rtc();
        for (Tau i = 0; i < max_iter; ++i) {
            Clock_cycle();
        }
        dt = rtc() - t0;
        if (dt >= mint)
            break;
        if (dt < 0.01) {
            max_iter *= 10;
        } else {
            max_iter = (max_iter * mint * 2.0) / dt;
        }
    }


    fprintf(stderr, "\n");
    fprintf(stderr, "Clock benchmark:\n");
    fprintf(stderr, "  max_iter is %lu\n", max_iter);
    fprintf(stderr, "  elapsed time is %.3f ms\n", dt * 1000.0);

    double              ns_per_call = dt * 1000000000.0 / max_iter;

    fprintf(stderr, "  time per count is %.3f ns\n", ns_per_call);
    fprintf(stderr, "\n");

    // typically at -O2 and -O3 this is 5-6 ns.
    // even at -g -O0, we should be around 12 ns,
    // fail this test if we hit 50 ns.
    assert(ns_per_call < 50.0);
}

void Clock_bist()
{
    Clock_init(1000, 18);

    TAU = 0;
    UNIT = 0;

    Tau                 rises = 0;
    Edge_rise(&CLOCK, clock_hi, &rises);

    Tau                 falls = 0;
    Edge_fall(&CLOCK, clock_lo, &falls);

    for (int i = 1; i <= 18; ++i) {
        Clock_cycle();
        assert(rises == i);
        assert(falls == i);
        assert(UNIT == i);
        assert(TAU == (i * 500) / 9);
    }

    assert(TAU == 1000);

    Clock_bench();
    printf("Clock_bist complete\n");
}
