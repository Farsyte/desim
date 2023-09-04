#include "cpu8080_reset.h"

#include <assert.h>

#include "stub.h"
#include "tau.h"

static void s_reset_done(Cpu8080 cpu, Cpu8080_phase ph)
{
    switch (ph) {
      default:
          break;
      case PHI1_RISE:
          Edge_lo(cpu->INT_RST);
          Edge_hi(cpu->RETM1);
          break;
      case PHI2_RISE:
          break;
      case PHI2_FALL:
          break;
    }
}

static void reset_fall(Cpu8080 cpu)
{
    *cpu->PC = 0;

    Edge_lo(cpu->SYNC);
    Edge_lo(cpu->DBIN);
    Edge_hi(cpu->WR_);
    Edge_lo(cpu->WAIT);

    cpu->state_next = s_reset_done;
}

static void s_reset_hold(Cpu8080 cpu, Cpu8080_phase ph)
{
    switch (ph) {
      default:
          break;
      case PHI1_RISE:
          Edge_hi(cpu->INT_RST);
          break;
      case PHI2_RISE:
          break;
      case PHI2_FALL:
          break;
    }
}

static void reset_rise(Cpu8080 cpu)
{
    *cpu->PC = 0;

    Edge_lo(cpu->SYNC);
    Edge_lo(cpu->DBIN);
    Edge_hi(cpu->WR_);
    Edge_lo(cpu->WAIT);

    cpu->state = s_reset_hold;
    cpu->state_next = s_reset_hold;
}

void Cpu8080_init_reset(Cpu8080 cpu)
{
    EDGE_RISE(cpu->RESET, reset_rise, cpu);
    EDGE_FALL(cpu->RESET, reset_fall, cpu);

    assert(Edge_get(cpu->RESET));

    reset_rise(cpu);
}
