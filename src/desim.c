#include <stdio.h>
#include <time.h>

#include "bist_macros.h"
#include "clock.h"
#include "cpu8080.h"
#include "ctl8228.h"
#include "edge.h"
#include "gen8224.h"
#include "rtc.h"
#include "tau.h"
#include "timing_check.h"

static void desim_bist()
{
    PRINT_TOP();
    rtc_bist();
    timing_check_bist();

    Tau_bist();
    Edge_bist();
    Clock_bist();
    Gen8224_bist();
    Ctl8228_bist();
    Cpu8080_bist();
    PRINT_END();
}

static void desim_bench()
{
    PRINT_TOP();

    Edge_bench();
    Clock_bench();
    Gen8224_bench();
    // Ctl8228_bench();
    Cpu8080_bench();
    PRINT_END();
}

int main(int argc, char **argv)
{
    PRINT_TOP();

    (void)argc;
    (void)argv;

    desim_bist();
    desim_bench();

    PRINT_END();
}
