#pragma once

// The Tau type defines how the simulation represents
// simulated time. It must be possible for values of this
// type to take on strictly ascending values as the
// simulation moves forward through discrete time steps.
//
// The value of a TAU is a number of nanoseconds.

typedef long Tau;

// The TAU global of type Tau represents the current simulated
// time for the engine. It increases as the simulation engine
// steps forward from one discrete time to the next, and remains
// constant while evaluating the simulation logic for each time.

extern Tau          TAU;

// The UNIT global of type unit_t represents the integer count
// of some basic time unit in the simulation that can be used
// neatly in data plotting code. It increments once for each
// cycle of the primary oscillator.

extern Tau          UNIT;

// provide a BIST that checks that we are good.
extern void         Tau_bist();
