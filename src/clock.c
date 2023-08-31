#include "clock.h"

#include <stdio.h>

Edge                CLOCK;

static Tau          p_num;
static Tau          p_den;
static Tau          excess;

// Initialize the clock with period (num/den) ns.
// Or think of this as (den*1000/num) MHz.
void Clock_init(Tau period_num, Tau period_den)
{
    Edge_init(&CLOCK);
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
    Edge_hi(&CLOCK);
    Edge_lo(&CLOCK);
    time_passes();
}
