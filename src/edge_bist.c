#include "edge.h"

#include <stdio.h>
#include <time.h>
#include <assert.h>

#include "stub.h"

const char          a1[] = "HI";
const char          a2[] = "LO";

typedef struct state_s {
    unsigned long       ticks;
    unsigned long       tocks;
} State[1];

static double rtc()
{
    struct timespec     tp[1];
    clock_gettime(CLOCK_MONOTONIC_RAW, tp);
    return tp->tv_sec + tp->tv_nsec / 1000000000.0;
}

static void tick(State arg)
{
    arg->ticks++;
}

static void tock(State arg)
{
    arg->tocks++;
}

void Edge_bench()
{
    Edge                e = { {"BENCHEDGE"} };
    State               s;

    Edge_init(e);

    for (unsigned long i = 0; i < 100; ++i) {
        EDGE_RISE(e, tick, s);
        EDGE_FALL(e, tock, s);
    }

    for (unsigned long i = 0; i < 10; ++i) {
        Edge_set(e, 1);
        Edge_set(e, 0);
    }

    unsigned long       max_iter = 1000;
    double              t0, dt;
    double              mint = 0.25;

    while (1) {
        s->ticks = 0;
        s->tocks = 0;

        t0 = rtc();
        for (unsigned long i = 0; i < max_iter; ++i) {
            Edge_set(e, 1);
            Edge_set(e, 0);
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
    fprintf(stderr, "Edge benchmark:\n");
    fprintf(stderr, "  count is %lu ticks, %lu tocks\n",
            s->ticks, s->tocks);
    fprintf(stderr, "  elapsed time is %.3f ms\n", dt * 1000.0);

    double              ns_per_call =
      dt * 1000000000.0 / (s->ticks + s->tocks);

    fprintf(stderr, "  time per count is %.3f ns\n", ns_per_call);
    fprintf(stderr, "\n");

    assert(s->ticks > 0);
    assert(s->tocks > 0);
    assert(s->ticks == s->tocks);
    // even at -g -O0, we should be around 2 ns,
    // fail this test if we hit 20 ns.
    assert(ns_per_call < 20.0);

}

static void s1(const char msg[])
{
    printf("%s: %s\n", __func__, msg);
}
static void s2(const char msg[])
{
    printf("%s: %s\n", __func__, msg);
}
static void s3(const char msg[])
{
    printf("%s: %s\n", __func__, msg);
}

void Edge_bist()
{
    Edge                a = { {"SIGA"} };
    Edge                b = { {"SIGB"} };
    Edge                c = { {"SIGC"} };

    Edge_init(a);
    Edge_init(b);
    Edge_init(c);

    EDGE_RISE(a, s1, "a↑");
    EDGE_RISE(a, s2, "a↑");
    EDGE_FALL(a, s3, "a↓");

    EDGE_RISE(b, s2, "b↑");
    EDGE_FALL(b, s3, "b↓");

    EDGE_RISE(c, s1, "c↑");
    EDGE_FALL(c, s3, "c↓");

    for (int i = 0; i < 3; ++i) {
        Edge_set(a, 1);
        Edge_set(b, 1);
        Edge_set(c, 1);
        Edge_set(a, 0);
        Edge_set(b, 0);
        Edge_set(c, 0);
    }

    // PASS if output matches the reference output.

    // TODO add unit tests for "Edge" facility
    Edge_bench();
    printf("Edge_bist complete\n");
}
