#include "cpu8080_hlt.h"

#include <assert.h>

#include "8080_instructions.h"
#include "8080_status.h"
#include "stub.h"
#include "tau.h"

static void s_ei(Cpu8080 cpu, Cpu8080_phase ph)
{
    switch (ph) {
      default:
          break;
      case PHI1_RISE:
          Edge_hi(cpu->RETM1);
          break;
      case PHI2_RISE:
          Edge_hi(cpu->INTE);
          break;
      case PHI2_FALL:
          break;
    }
}

static void s_di(Cpu8080 cpu, Cpu8080_phase ph)
{
    switch (ph) {
      default:
          break;
      case PHI1_RISE:
          Edge_hi(cpu->RETM1);
          break;
      case PHI2_RISE:
          Edge_lo(cpu->INTE);
          break;
      case PHI2_FALL:
          break;
    }
}

void Cpu8080_init_int(Cpu8080 cpu)
{
    cpu->M1T4[I8080_DI] = s_di;
    cpu->M1T4[I8080_EI] = s_ei;
}
