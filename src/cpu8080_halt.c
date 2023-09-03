#include "cpu8080_halt.h"

#include <assert.h>

#include "8080_instructions.h"
#include "8080_status.h"
#include "stub.h"
#include "tau.h"

static void s_halt_M2TW(Cpu8080 cpu, Cpu8080_phase ph)
{

    switch (ph) {
      default:
          break;
      case PHI1_RISE:
          Edge_hi(cpu->WAIT);
          // TODO respond to cpu->INT
          break;
      case PHI2_RISE:
          // TODO respond to cpu->HOLD
          break;
      case PHI2_FALL:
          break;
    }
    // TODO when we leave M2TW, release WAIT on rising PHI1 of the next state.
}

static void s_halt_M2T2(Cpu8080 cpu, Cpu8080_phase ph)
{
    switch (ph) {
      default:
          break;
      case PHI1_RISE:
          break;
      case PHI2_RISE:
          Edge_lo(cpu->SYNC);
          // 8080 floats Addr
          // 8080 floats Data
          break;
      case PHI2_FALL:
          cpu->state_next = s_halt_M2TW;
          break;
    }
}

static void s_halt_M2T1(Cpu8080 cpu, Cpu8080_phase ph)
{
    switch (ph) {
      default:
          break;
      case PHI1_RISE:
          break;
      case PHI2_RISE:
          *cpu->Addr = *cpu->PC;
          *cpu->Data = STATUS_HALTACK;
          Edge_hi(cpu->SYNC);
          break;
      case PHI2_FALL:
          cpu->state_next = s_halt_M2T2;
          break;
    }
}

static void s_halt_M1T4(Cpu8080 cpu, Cpu8080_phase ph)
{
    switch (ph) {
      default:
          break;
      case PHI1_RISE:
          break;
      case PHI2_RISE:
          break;
      case PHI2_FALL:
          cpu->state_next = s_halt_M2T1;
          break;
    }
}

void Cpu8080_init_halt(Cpu8080 cpu)
{
    cpu->M1T4[I8080_HLT] = s_halt_M1T4;
}
