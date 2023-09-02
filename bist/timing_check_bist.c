#include "timing_check.h"

#include <stdio.h>
#include <assert.h>

#include "bist_macros.h"

void timing_check_bist()
{
    PRINT_TOP();

    TimingCheck         e = { {"τ", 60, 90} };

    timing_check_init(e);

    assert(TIMING_CHECK_NO == timing_check_fails(e));

    timing_check_print(e);      // indicate failure code NO

    timing_check_start(e, 0);
    timing_check_start(e, 100);
    timing_check_record(e, 170);
    timing_check_start(e, 200);
    timing_check_record(e, 280);

    assert(!timing_check_fails(e));

    timing_check_print(e);      // show a good run, everyone in range

    timing_check_start(e, 300);
    timing_check_record(e, 350);

    timing_check_print(e);      // indicate failure code LB
    assert(TIMING_CHECK_LB == timing_check_fails(e));

    timing_check_start(e, 400);
    timing_check_record(e, 499);

    timing_check_print(e);      // indicate failure codes LB UB
    assert(TIMING_CHECK_UBLB == timing_check_fails(e));

    PRINT_END();
}
