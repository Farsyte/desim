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
          break;
      case PHI2_RISE:
          break;
      case PHI2_FALL:
          cpu->state_next = cpu->M1T1;
          break;
    }
}

static void s_di(Cpu8080 cpu, Cpu8080_phase ph)
{
    switch (ph) {
      default:
          break;
      case PHI1_RISE:
          break;
      case PHI2_RISE:
          break;
      case PHI2_FALL:
          cpu->state_next = cpu->M1T1;
          break;
    }
}

void Cpu8080_init_int(Cpu8080 cpu)
{
    cpu->M1T4[I8080_DI] = s_di;
    cpu->M1T4[I8080_EI] = s_ei;
}
