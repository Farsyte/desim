#include "cpu8080_hlt.h"

#include <assert.h>

#include "8080_instructions.h"
#include "8080_status.h"
#include "stub.h"
#include "tau.h"

static void s_hlt_M2TW(Cpu8080 cpu, Cpu8080_phase ph)
{
    switch (ph) {
      default:
          break;
      case PHI1_RISE:
          Edge_hi(cpu->WAIT);
          break;
      case PHI2_RISE:
          break;
      case PHI2_FALL:
          break;
    }
}

static void s_hlt_M2T2(Cpu8080 cpu, Cpu8080_phase ph)
{
    switch (ph) {
      default:
          break;
      case PHI1_RISE:
          break;
      case PHI2_RISE:
          Edge_lo(cpu->SYNC);
          break;
      case PHI2_FALL:
          cpu->state_next = s_hlt_M2TW;
          break;
    }
}

static void s_hlt_M2T1(Cpu8080 cpu, Cpu8080_phase ph)
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
          cpu->state_next = s_hlt_M2T2;
          break;
    }
}

static void s_hlt_M1T4(Cpu8080 cpu, Cpu8080_phase ph)
{
    switch (ph) {
      default:
          break;
      case PHI1_RISE:
          break;
      case PHI2_RISE:
          break;
      case PHI2_FALL:
          cpu->state_next = s_hlt_M2T1;
          break;
    }
}

void Cpu8080_init_hlt(Cpu8080 cpu)
{
    cpu->M1T4[I8080_HLT] = s_hlt_M1T4;
}
