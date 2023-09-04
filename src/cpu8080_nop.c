#include "cpu8080_nop.h"

#include "8080_instructions.h"

static void         s_nop(Cpu8080 cpu, Cpu8080_phase ph);

void Cpu8080_init_nop(Cpu8080 cpu)
{
    cpu->M1T4[I8080_NOP] = s_nop;
}

static void s_nop(Cpu8080 cpu, Cpu8080_phase ph)
{
    switch (ph) {
      case PHI2_RISE:
          Edge_hi(cpu->RETM1);
          break;
    }
}
