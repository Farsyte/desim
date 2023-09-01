#include <stdio.h>
#include <time.h>

#include "tau.h"
#include "edge.h"
#include "clock.h"
#include "rtc.h"

#include "timing_check.h"

#include "gen8224.h"
#include "ctl8228.h"

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    Tau_bist();
    Edge_bist();
    Clock_bist();
    timing_check_bist();
    Gen8224_bist();
    Ctl8228_bist();

    rtc_bist();

    printf("%s complete\n", argv[0]);
}
