#include <assert.h>
#include <stdio.h>
#include "bist_macros.h"
#include "clock.h"
#include "timing_check.h"

void timing_check_bist()
{
    PRINT_TOP();

    printf("\n");

    Clock_init(18.00);
    Clock_cycle_by(9);

    // 55 111 166 222 277 333 388 444 500 ...

    // allow 3 or 4 units.
    TimingCheck         e = { {"τ", 150, 250} };

    timing_check_init(e);

    assert(TIMING_CHECK_NO == timing_check_fails(e));

    timing_check_print(e);      // indicate failure code NO

    // gather some non-failure data.

    timing_check_start(e);
    Clock_cycle_by(9);
    timing_check_start(e);
    Clock_cycle_by(3);
    timing_check_record(e);
    Clock_cycle_by(6);
    timing_check_start(e);
    Clock_cycle_by(4);
    timing_check_record(e);
    Clock_cycle_by(5);

    assert(!timing_check_fails(e));

    timing_check_print(e);      // show a good run, everyone in range

    // induce a lower bound failure.

    Clock_cycle_by(9);
    timing_check_start(e);
    Clock_cycle_by(2);
    timing_check_record(e);
    Clock_cycle_by(7);

    timing_check_print(e);      // indicate failure code LB
    assert(TIMING_CHECK_LB == timing_check_fails(e));

    // induce an upper bound failure.

    Clock_cycle_by(9);
    timing_check_start(e);
    Clock_cycle_by(5);
    timing_check_record(e);
    Clock_cycle_by(4);

    timing_check_print(e);      // indicate failure codes LB UB
    assert(TIMING_CHECK_UBLB == timing_check_fails(e));

    PRINT_END();
}
