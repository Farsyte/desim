#include "timing_check.h"
#include <assert.h>
#include <stdio.h>
#include "clock.h"
#include "util.h"

#define UNITS_TO_NS(u)	((Tau)(u * 1000.0 / Clock_MHz))

void timing_check_init(TimingCheck e)
{
    assert(e);
    assert(e->name);
    assert(e->name[0]);
    assert(e->ub > e->lb);
    assert(e->ub < 1000000);

    e->lo = e->ub;
    e->hi = e->lb;
}

void timing_check_start(TimingCheck e)
{
    e->from = UNIT;
}

void timing_check_record(TimingCheck e)
{
    if (e->from <= 0)
        return;
    unsigned            c = e->count;
    Tau                 elapsed = UNIT - e->from;
    if (!c || e->lo > elapsed)
        e->lo = elapsed;
    if (!c || e->hi < elapsed)
        e->hi = elapsed;
    e->from = 0;
    e->count = c + 1;
}

unsigned timing_check_fails(TimingCheck e)
{
    unsigned            ret = 0;

    if (!e->count)
        return TIMING_CHECK_NO;

    if (e->lb && UNITS_TO_NS(e->lo) < e->lb)
        ret |= TIMING_CHECK_LB;
    if (e->ub && UNITS_TO_NS(e->hi) > e->ub)
        ret |= TIMING_CHECK_UB;

    return ret;
}

void timing_check_print(TimingCheck e)
{
    unsigned            fb = timing_check_fails(e);
    printf("TimingCheck: %s", e->name);

    if (e->lb) {
        if (e->ub) {
            printf(" allowed %lu to %lu ns", e->lb, e->ub);
        } else {
            printf(" at least %lu ns", e->lb);
        }
    } else {
        if (e->ub) {
            printf(" at most %lu ns", e->ub);
        } else {
            printf(" no bounds checked");
        }
    }
    if (fb & TIMING_CHECK_NO) {
        printf(" -- FAIL, NO samples observed");
    } else {
        printf(", saw %lu to %lu ns", UNITS_TO_NS(e->lo), UNITS_TO_NS(e->hi));
        if (fb) {
            printf(" -- FAIL,");
            if (fb & TIMING_CHECK_LB)
                printf(" LB");
            if (fb & TIMING_CHECK_UB)
                printf(" UB");
            printf(" timing violation(s).");
        }
    }
    printf("\n");
}
