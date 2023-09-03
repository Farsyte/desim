#include "clock.h"

#include <stdio.h>

#include "stub.h"
#include "util.h"

Edge                CLOCK = { {"CLOCK"} };

// static init fields to match an 18.00 MHz clock

double              Clock_MHz = 18.00;
double              Clock_ns = 1000.0 / 18.00;  // TODO elminate this cruft

Tau Clock_nsec()
{                       // TODO elminate this cruft
    return UNIT * 1000 / Clock_MHz;     // TODO elminate this cruft
}                       // TODO elminate this cruft

double Clock_usec()
{
    return UNIT / Clock_MHz;
}

// Initialize the clock with period (num/den) ns.
// Or think of this as (den*1000/num) MHz.
void Clock_init(double MHz)
{
    UNIT = 0;

    Clock_MHz = MHz;
    Clock_ns = 1000.0 / MHz;    // TODO elminate this cruft

    CLOCK->name = format("CLOCK_%.0fMHz_%.0fNS", Clock_MHz, Clock_ns);

    Edge_init(CLOCK);
}

void Clock_cycle()
{
    Edge_hi(CLOCK);
    Edge_lo(CLOCK);
    UNIT++;
}

void Clock_cycle_to(Tau uu)
{
    while (UNIT < uu)
        Clock_cycle();
}

void Clock_cycle_by(Tau uu)
{
    Clock_cycle_to(UNIT + uu);
}
