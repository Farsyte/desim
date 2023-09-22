#pragma once
#include "sim_types.h"
#include "tau.h"

typedef struct {
    Name                name;
    Tau                 lb, ub;                 // ns

    Tau                 lo, hi;
    Tau                 from;
    unsigned            count;
}                  *pTimingCheck, TimingCheck[1];

#define TIMING_CHECK_NO		0x01
#define TIMING_CHECK_LB		0x02
#define TIMING_CHECK_UB		0x04
#define TIMING_CHECK_UBLB 	0x06

extern void         timing_check_init(TimingCheck e);

extern void         timing_check_start(TimingCheck e);
extern void         timing_check_record(TimingCheck e);

extern void         timing_check_print(TimingCheck e);
extern unsigned     timing_check_fails(TimingCheck e);

extern void         timing_check_bist();
