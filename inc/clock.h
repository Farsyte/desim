#pragma once

#include "edge.h"

extern Edge         CLOCK;

extern double       Clock_MHz;

extern void         Clock_init(double MHz);
extern void         Clock_cycle();
extern void         Clock_cycle_by(Tau ns);
extern void         Clock_cycle_to(Tau ns);
extern void         Clock_bist();
extern void         Clock_bench();
