#include "rtc.h"

#include <time.h>

double rtc()
{
    struct timespec     tp[1];
    clock_gettime(CLOCK_MONOTONIC_RAW, tp);
    return tp->tv_sec + tp->tv_nsec / 1000000000.0;
}
