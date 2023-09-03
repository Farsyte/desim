#include "cpu8080_m1t1.h"

#include "cpu8080.h"

static void Cpu8080_update_M1T1(Cpu8080 cpu)
{
    // TODO observe signals that, if active, modify the behavior
    // of the next M1 cycle T1 state.
    cpu->M1T1 = cpu->fetch;
}

void Cpu8080_init_M1T1(Cpu8080 cpu)
{
    // TODO subscribe to signal edges that cause the CPU to change
    // the behavior of the next M1 cycle T1 state
    Cpu8080_update_M1T1(cpu);
}
