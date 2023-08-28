#include "edge.hh"

#include <cassert>
#include <cstdio> // c++ <streams> are too painful for words.
//#include <iostream> // but sometimes I have to use them

#include "timer.hh"

static int      rc[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
static unsigned t     = 0;

static void check_order()
{
    Edge e;

    // use "t" to verify we are running in FIFO order,
    // in case the implementation changes to LIFO order.
    // We might do LIFO but do not do it accidentally.

    e.rise_cb(ACTION(rc[0]++; t++; assert(t == 1)));
    e.rise_cb(ACTION(rc[1]++; t++; assert(t == 2)));
    e.rise_cb(ACTION(rc[2]++; t++; assert(t == 3)));
    e.rise_cb(ACTION(rc[3]++; t++; assert(t == 4)));
    e.fall_cb(ACTION(rc[4]++; t++; assert(t == 5)));
    e.fall_cb(ACTION(rc[5]++; t++; assert(t == 6)));
    e.fall_cb(ACTION(rc[6]++; t++; assert(t == 7)));
    e.fall_cb(ACTION(rc[7]++; t++; assert(t == 8); t = 0));

    for (int rep = 0; rep < 4; ++rep)
        e.hi(), e.lo();

    // verify each lambda was called the right number of times.
    assert(rc[0] == 4);
    assert(rc[1] == 4);
    assert(rc[2] == 4);
    assert(rc[3] == 4);
    assert(rc[4] == 4);
    assert(rc[5] == 4);
    assert(rc[6] == 4);
    assert(rc[7] == 4);
}

static unsigned max_count  = 1000;
static unsigned call_count = 0;

void inc_count()
{
    ++call_count;
}

static double check_timing()
{
    // now do a timing test where we have one signal that
    // is timing pushd, and a bunch that happen one
    // immediately after the other.

    Edge e;

    // set up a dozen consumers of the rising and falling edges,
    // each of which just increments call_count.

    for (int i = 0; i < 12; ++i) {
        Edge_RISE(&e, inc_count);
        Edge_FALL(&e, inc_count);
    }

    Timer t;

    call_count = 0;

    t.tick();
    for (unsigned rep = 0; rep < max_count; ++rep)
        e.hi(), e.lo();
    t.tock();

    unsigned long us_total = t.microseconds();
    if (us_total < 100) {
        max_count *= 10;
        return check_timing();
    }
    if (us_total < 100000) {
        max_count = (max_count * 200000) / us_total;
        return check_timing();
    }

    double ns_avg = us_total * 1000.0 / call_count;

    //    printf("total time: %lu ms\n", us_total/1000);
    //    printf("call count: %u\n", call_count);
    //    printf("avg time: %.1f ns\n", ns_avg);

    return ns_avg;
}

template <typename T>
T min(T a, T b)
{
    return a < b ? a : b;
}

void Edge::bist()
{
    check_order();

    // run the timing three times and take the best,
    // just in case something in the build and test
    // environment paused us.

//    max_count = 1000;

//    double t1 = check_timing();
//    double t2 = check_timing();
//    double t3 = check_timing();
//    if (t1 > t2)
//        t1 = t2;
//    if (t1 > t3)
//        t1 = t3;
//    fprintf(stderr, "Edge: %.1f ns (wall)\n", t1);

    // Currently this averages ~18-20ns per service call
    // on my "fragbox" (AMD Ryzen 9 5950x, 2.2 to 4.6 GHz)
    // when compiled "-g -O0" and ~1.3ns per service call
    // when compiled "-Ofast".
    //
    // If it grows to 50ns, something bad has happened.

//    assert(t1 < 50);

    printf("Edge::bist complete.\n");
}
