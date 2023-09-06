#include "cpu8080_m1t1.h"
#include "cpu8080.h"

static void Cpu8080_maybe_int(Cpu8080 cpu)
{
    if (Edge_get(cpu->INT) && Edge_get(cpu->INTE))
        cpu->M1T1_from_int = cpu->intack;
    else
        cpu->M1T1_from_int = cpu->fetch;
}

void Cpu8080_init_M1T1(Cpu8080 cpu)
{
    // Maintain cpu->M1T1_from_int as the M1T1 state function that is
    // "intack" if we have an interrupt enabled and pending, and is
    // "fetch" if not.

    EDGE_RISE(cpu->INT, Cpu8080_maybe_int, cpu);
    EDGE_FALL(cpu->INT, Cpu8080_maybe_int, cpu);
    EDGE_RISE(cpu->INTE, Cpu8080_maybe_int, cpu);
    EDGE_FALL(cpu->INTE, Cpu8080_maybe_int, cpu);

    Cpu8080_maybe_int(cpu);
}
