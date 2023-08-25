#pragma once

// The tau_t type defines how the simulation represents
// simulated time. It must be possible for values of this
// type to take on strictly ascending values as the
// simulation moves forward through discrete time steps.

typedef unsigned long tau_t;

// The TAU global of type tau_t represents the current simulated
// time for the engine. It increases as the simulation engine
// steps forward from one discrete time to the next, and remains
// constant while evaluating the simulation logic for each time.

extern tau_t TAU;
