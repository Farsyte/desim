#include "cpu8080_reset.h"

#include <assert.h>

#include "stub.h"
#include "tau.h"

static void s_reset(Cpu8080 cpu, Cpu8080_phase ph)
{
    if (ph == PHI1_RISE) {
        Edge_lo(cpu->WAIT);
        if (!Edge_get(cpu->RESET)) {
            cpu->state_next = cpu->M1T1;
        }
    }
}

static void reset_rise(Cpu8080 cpu)
{
    *cpu->PC = 0;

    Edge_lo(cpu->SYNC);
    Edge_lo(cpu->DBIN);
    Edge_hi(cpu->WR_);

    cpu->state = s_reset;
    cpu->state_next = s_reset;
}

void Cpu8080_init_reset(Cpu8080 cpu)
{
    EDGE_RISE(cpu->RESET, reset_rise, cpu);

    assert(Edge_get(cpu->RESET));

    reset_rise(cpu);
}
