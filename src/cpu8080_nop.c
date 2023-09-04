#include "cpu8080_nop.h"

#include "8080_instructions.h"

static void s_nop(Cpu8080 cpu, Cpu8080_phase ph)
{
    switch (ph) {
      default:
          break;
      case PHI1_RISE:
          Edge_hi(cpu->RETM1);
          break;
      case PHI2_RISE:
          break;
      case PHI2_FALL:
          break;
    }
}

void Cpu8080_init_nop(Cpu8080 cpu)
{
    cpu->M1T4[I8080_NOP] = s_nop;
}
