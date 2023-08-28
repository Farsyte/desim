#include "actions.hh"

#include <cassert>
#include <cstdio> // c++ <streams> are too painful for words.
//#include <iostream> // but sometimes I have to use them

#include "timer.hh"

static void check_order()
{
    Actions a;

    int rc[4] = { 0, 0, 0, 0 };

    unsigned t = 0;

    // use "t" to verify we are running in FIFO order,
    // in case the implementation changes to LIFO order.
    // We might do LIFO but do not do it accidentally.

    a.push([&] { t++; rc[0] ++; assert(t == 1); });
    a.push([&] { t++; rc[1] ++; assert(t == 2); });
    a.push([&] { t++; rc[2] ++; assert(t == 3); });
    a.push([&] { t++; rc[3] ++; assert(t == 4); });

    a.exec();

    // verify each lambda was called once.
    assert(rc[0] == 1);
    assert(rc[1] == 1);
    assert(rc[2] == 1);
    assert(rc[3] == 1);
}

static unsigned call_count = 0;
static unsigned max_count  = 0;

static void cycle(Actions* a)
{
    ++call_count;
    if (call_count < max_count)
        a->push(ACTION(cycle(a)));
}

static double check_timing()
{
    // now do a timing test where we have one signal that
    // is timing pushd, and a bunch that happen one
    // immediately after the other.

    Actions a;

    call_count = 0;

    a.push([&] { cycle(&a); });

    Timer t;

    t.tick();
    while (!a.empty())
        a.exec();
    t.tock();

    unsigned long ms_total = t.microseconds();
    if (ms_total < 100) {
        max_count *= 10;
        return check_timing();
    }
    if (ms_total < 100000) {
        max_count = (max_count * 200000) / ms_total;
        return check_timing();
    }

    double ns_avg = ms_total * 1000.0 / call_count;

    //    printf("total time: %lu ms\n", ms_total);
    //    printf("call count: %u\n", call_count);
    //    printf("avg time: %.1f ns\n", ns_avg);

    return ns_avg;
}

template <typename T>
T min(T a, T b)
{
    return a < b ? a : b;
}

void Actions::bist()
{
    check_order();

//    // run the timing three times and take the best,
//    // just in case something in the build and test
//    // environment paused us.
//    double mt = 0;
//
//    max_count = 1000;
//
//    // if check_timing notices elapsed time is too short,
//    // it adjusts max_count and returns -1 for a rerun.
//    mt = check_timing();
//    mt = min(mt, check_timing());
//    mt = min(mt, check_timing());
//
//    fprintf(stderr, "Actions: %.1f ns (wall)\n", mt);
//
//    // Currently this averages ~90-100ns per service call
//    // on my "fragbox" (AMD Ryzen 9 5950x, 2.2 to 4.6 GHz).
//    //
//    // If it grows to 1μs, something bad has happened.
//
//    assert(mt < 1000);

    printf("Actions::bist complete.\n");
}
