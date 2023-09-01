#include "clock.h"

#include <stdio.h>

#include "stub.h"
#include "util.h"

Edge                CLOCK = { {"CLOCK"} };

// static init fields to match an 18.00 MHz clock
static Tau          p_num = 1000;
static Tau          p_den = 18;
static Tau          excess = 0;

// Initialize the clock with period (num/den) ns.
// Or think of this as (den*1000/num) MHz.
void Clock_init(Tau period_num, Tau period_den)
{
    TAU = 0;
    UNIT = 0;

    CLOCK->name = format("CLOCK_%.0fMHz_%.0fNS",
                         p_den * 1000.0 / p_num, p_num * 1.0 / p_den);

    Edge_init(CLOCK);
    p_num = period_num;
    p_den = period_den;
    excess = 0;
}

static void time_passes()
{
    excess += p_num;
    TAU += excess / p_den;
    excess %= p_den;
    UNIT++;
}

void Clock_cycle()
{
    Edge_hi(CLOCK);
    Edge_lo(CLOCK);
    time_passes();
}
