#include "rtc.h"

#include <time.h>

Tau rtc_ns()
{
    struct timespec     tp[1];
    clock_gettime(CLOCK_MONOTONIC_RAW, tp);
    return tp->tv_sec * 1000000000 + tp->tv_nsec;
}
