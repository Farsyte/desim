#include "timing_check.h"

#include <assert.h>
#include <stdio.h>

#include "util.h"

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

void timing_check_start(TimingCheck e, Tau t0)
{
    e->from = t0;
}

unsigned timing_check_fails(TimingCheck e)
{
    unsigned            ret = 0;
    if (e->hi < e->lo)
        ret |= TIMING_CHECK_NO;
    if (e->lo < e->lb)
        ret |= TIMING_CHECK_LB;
    if (e->hi > e->ub)
        ret |= TIMING_CHECK_UB;
    return ret;
}

void timing_check_record(TimingCheck e, Tau tF)
{
    if (e->from <= 0)
        return;
    e->elapsed = tF - e->from;
    if (e->lo > e->elapsed)
        e->lo = e->elapsed;
    if (e->hi < e->elapsed)
        e->hi = e->elapsed;
    e->from = 0;
}

void timing_check_print(TimingCheck e)
{
    unsigned            fb = timing_check_fails(e);
    printf("TimingCheck: %s", e->name);

    printf(" allowed %.3f to %.3f μs",
           e->lb / 1000.0, e->ub / 1000.0);
    printf(" saw %.3f to %.3f μs",
           e->lo / 1000.0, e->hi / 1000.0);

    if (fb & TIMING_CHECK_NO)
        printf(" NO");
    if (fb & TIMING_CHECK_LB)
        printf(" LB");
    if (fb & TIMING_CHECK_UB)
        printf(" UB");

    printf("\n");
}
