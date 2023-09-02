#include <stdio.h>
#include <time.h>

#include "tau.h"
#include "edge.h"
#include "clock.h"
#include "rtc.h"

#include "timing_check.h"

#include "gen8224.h"
#include "ctl8228.h"
#include "cpu8080.h"

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    rtc_bist();
    timing_check_bist();

    Tau_bist();
    Edge_bist();
    Clock_bist();
    Gen8224_bist();
    Ctl8228_bist();
    Cpu8080_bist();

    printf("%s complete\n", argv[0]);
}
