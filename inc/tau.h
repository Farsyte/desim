#pragma once

// The Tau type defines how the simulation represents
// simulated time. It must be possible for values of this
// type to take on strictly ascending values as the
// simulation moves forward through discrete time steps.

typedef long Tau;

// The TU macro evaluates to the simulation time
// in floating point microseconds.

#define	TU	(Clock_usec())

// The TAU macro evaluates to the simulation time
// in 64-bit integer ns.
// The use of TAU is [[DEPRECATED]] -- please use TU.

#define	TAU	(Clock_nsec())

// The UNIT global of type unit_t represents the integer count
// of some basic time unit in the simulation that can be used
// neatly in data plotting code. It increments once for each
// cycle of the primary oscillator.

extern Tau          UNIT;

// provide a BIST that checks that we are good.
extern void         Tau_bist();

extern Tau          Clock_nsec();
extern double       Clock_usec();
