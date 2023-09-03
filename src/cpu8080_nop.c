#include "cpu8080_nop.h"

#include "8080_instructions.h"

static void s_nop(Cpu8080 cpu, Cpu8080_phase ph)
{
    if (ph == PHI2_FALL) {
        cpu->state_next = cpu->M1T1;
    }
}

void Cpu8080_init_nop(Cpu8080 cpu)
{
    cpu->M1T4[I8080_NOP] = s_nop;
}
