#pragma once

class VerifyElapsedTime {
public:
    VerifyElapsedTime(const char* name, double lb, double ub)
        : name(name)
        , lb(lb)
        , ub(ub)
        , lo(ub)
        , hi(lb)
        , from(0)
        , elapsed(0)
    {
    }

    const char* name;
    double lb;
    double ub;

    double lo;
    double hi;

    double from;
    double elapsed;

    void start(double ns)
    {
        from = ns;
    }

    void record(double ns)
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

    void print()
    {
        if (hi < lo)
            return;
        if (lo < lb || hi > ub) {
            printf("VerifyElapsedTime: %8.1f lb, %6.1f ub, %8.1f lo, %6.1f hi, %s\n",
                lb, ub, lo, hi, name);
        } else {
            printf("VerifyElapsedTime: %8.1f lb, %6.1f ub, %8.1f lo, %6.1f hi, %s\n",
                lb, ub, lo, hi, name);
        }
    }
};
