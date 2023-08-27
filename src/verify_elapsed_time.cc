#include "verify_elapsed_time.hh"

#include <cstdio>

VerifyElapsedTime::VerifyElapsedTime(const char* name, double lb, double ub)
    : name(name)
    , lb(lb)
    , ub(ub)
    , lo(ub)
    , hi(lb)
    , from(0)
    , elapsed(0)
{
}

void VerifyElapsedTime::start(double ns)
{
    from = ns;
}

void VerifyElapsedTime::record(double ns)
{
    if (from <= 0)
        return;

    elapsed = ns - from;
    // if (lb > 0)
    //     assert(elapsed >= lb);
    // if (ub > 0)
    //     assert(elapsed <= ub);

    if (lo > elapsed)
        lo = elapsed;
    if (hi < elapsed)
        hi = elapsed;

    from = 0;
}

void VerifyElapsedTime::print()
{
    printf("VerifyElapsedTime: %8.1f lb, %6.1f ub, %8.1f lo, %6.1f hi, %s",
        lb, ub, lo, hi, name);
    if (hi < lo)
        printf(" NO");
    if (lo < lb)
        printf(" LB");
    if (hi > ub)
        printf(" UB");
    printf("\n");
}
