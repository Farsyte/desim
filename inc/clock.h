#pragma once

#include "edge.h"

extern Edge         CLOCK;

extern void         Clock_init(Tau period_num, Tau period_den);
extern void         Clock_cycle();
extern void         Clock_bist();
