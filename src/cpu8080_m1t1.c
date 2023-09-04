#include "cpu8080_m1t1.h"

#include "cpu8080.h"

static void Cpu8080_update_M1T1(Cpu8080 cpu)
{
    // TODO observe more signals that, if active, modify the behavior
    // of the next M1 cycle T1 state.

    if (Edge_get(cpu->INT) && Edge_get(cpu->INTE))
        cpu->M1T1 = cpu->intack;
    else
        cpu->M1T1 = cpu->fetch;
}

void Cpu8080_init_M1T1(Cpu8080 cpu)
{
    // TODO subscribe to signal edges that cause the CPU to change
    // the behavior of the next M1 cycle T1 state

    EDGE_RISE(cpu->INT, Cpu8080_update_M1T1, cpu);
    EDGE_FALL(cpu->INT, Cpu8080_update_M1T1, cpu);
    EDGE_RISE(cpu->INTE, Cpu8080_update_M1T1, cpu);
    EDGE_FALL(cpu->INTE, Cpu8080_update_M1T1, cpu);

    Cpu8080_update_M1T1(cpu);
}
