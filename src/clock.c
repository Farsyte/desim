#include "clock.h"

#include <stdio.h>

#include "stub.h"
#include "util.h"

Edge                CLOCK = { {"CLOCK"} };

// static init fields to match an 18.00 MHz clock
// clock period is p_int + p_num/p_den ns.

static int          p_int = 55;
static int          p_num = 5;
static int          p_den = 9;
static int          excess = 0;

static double       freq_mhz = 18.00;
static double       period_ns = 1000.0 / 18.00;

// Initialize the clock with period (num/den) ns.
// Or think of this as (den*1000/num) MHz.
void Clock_init(Tau period_num, Tau period_den)
{
    TAU = 0;
    UNIT = 0;

    period_ns = period_num * 1.0 / period_den;
    freq_mhz = period_den * 1000.0 / period_num;

    CLOCK->name = format("CLOCK_%.0fMHz_%.0fNS", freq_mhz, period_ns);

    Edge_init(CLOCK);
    p_int = period_num / period_den;
    p_num = period_num % period_den;
    p_den = period_den;
    excess = 0;
}

static void time_passes()
{

    // This function is timing critical.
    // 

    excess += p_num;
    if (excess < p_den) {
        TAU += p_int;
    } else {
        TAU += p_int + 1;
        excess -= p_den;
    }
    UNIT++;
}

void Clock_cycle()
{
    Edge_hi(CLOCK);
    Edge_lo(CLOCK);
    time_passes();
}

void Clock_cycle_to(Tau ns)
{
    while (TAU < ns)
        Clock_cycle();
}

void Clock_cycle_by(Tau ns)
{
    Clock_cycle_to(TAU + ns);
}
