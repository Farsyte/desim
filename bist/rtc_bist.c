#include <assert.h>
#include <stdio.h>

#include "bist_macros.h"
#include "rtc.h"

void rtc_bist()
{
    PRINT_TOP();

    Tau                 t0 = rtc_ns();
    Tau                 t1 = rtc_ns();
    Tau                 dt = t1 - t0;

    fprintf(stderr, "for two sequential rtc_ns calls:\n");
    fprintf(stderr, "  t0: %9lu ns since init\n", t0);
    fprintf(stderr, "  t1: %9lu ns since init\n", t1);
    fprintf(stderr, "  dt: %9lu ns\n", dt);

    assert(t0 < t1);
    assert(dt < 1000);

    // If either of the two triggers, the test logic
    // will need to be made more complicated to handle
    // being timesliced in the middle, or managing to
    // do two rtc_ns calls within a time period that
    // the host system thinks is zero elapsed time.

    PRINT_END();
}
