#include <assert.h>
#include <stdio.h>

#include "bist_macros.h"
#include "edge.h"
#include "rtc.h"
#include "stub.h"

const char          a1[] = "HI";
const char          a2[] = "LO";

typedef struct state_s {
    unsigned long       ticks;
    unsigned long       tocks;
} State[1];

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
    PRINT_TOP();

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
    Tau                 t0, dt;
    Tau                 mint = 25000000;

    while (1) {
        s->ticks = 0;
        s->tocks = 0;

        t0 = rtc_ns();
        for (unsigned long i = 0; i < max_iter; ++i) {
            Edge_set(e, 1);
            Edge_set(e, 0);
        }
        dt = rtc_ns() - t0;
        if (dt >= mint)
            break;
        if (dt < mint / 10) {
            max_iter *= 10;
        } else {
            max_iter = (max_iter * mint * 2.0) / dt;
        }
    }

    fprintf(stderr, "\n");
    fprintf(stderr, "Edge benchmark:\n");
    fprintf(stderr, "  count is %lu ticks, %lu tocks\n",
            s->ticks, s->tocks);
    fprintf(stderr, "  elapsed time is %.3f ms\n", dt / 1000000.0);

    double              ns_per_call = dt * 1.0 / (s->ticks + s->tocks);

    fprintf(stderr, "  time per count is %.3f ns\n", ns_per_call);
    fprintf(stderr, "\n");

    assert(s->ticks > 0);
    assert(s->tocks > 0);
    assert(s->ticks == s->tocks);
    // even at -g -O0, we should be around 2 ns,
    // fail this test if we hit 20 ns.
    assert(ns_per_call < 20.0);

    PRINT_END();
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
    PRINT_TOP();

    printf("\n");

    Edge                a = { {"SIGA"} };
    Edge                b = { {"SIGB"} };
    Edge                c = { {"SIGC"} };

    Edge_init(a);
    Edge_init(b);
    Edge_init(c);

    assert(!Edge_get(a));
    assert(!Edge_get(b));
    assert(!Edge_get(c));

    Edge_set(a, 1);
    Edge_set(b, 1);
    Edge_set(c, 1);

    assert(Edge_get(a));
    assert(Edge_get(b));
    assert(Edge_get(c));

    Edge_set(a, 1);
    Edge_set(b, 1);
    Edge_set(c, 1);

    assert(Edge_get(a));
    assert(Edge_get(b));
    assert(Edge_get(c));

    Edge_set(a, 0);
    Edge_set(b, 0);
    Edge_set(c, 0);

    assert(!Edge_get(a));
    assert(!Edge_get(b));
    assert(!Edge_get(c));

    Edge_set(a, 0);
    Edge_set(b, 0);
    Edge_set(c, 0);

    assert(!Edge_get(a));
    assert(!Edge_get(b));
    assert(!Edge_get(c));

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

    PRINT_END();
}
