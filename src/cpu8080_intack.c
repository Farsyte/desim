#include "cpu8080_intack.h"

#include <stdlib.h>

#include "8080_status.h"
#include "stub.h"

static void         s_intack_T1(Cpu8080 cpu, Cpu8080_phase ph);
static void         s_intack_T2(Cpu8080 cpu, Cpu8080_phase ph);
static void         s_intack_TW(Cpu8080 cpu, Cpu8080_phase ph);
static void         s_intack_T3(Cpu8080 cpu, Cpu8080_phase ph);

void Cpu8080_init_intack(Cpu8080 cpu)
{
    cpu->intack = s_intack_T1;
}

static void s_intack_T1(Cpu8080 cpu, Cpu8080_phase ph)
{
    switch (ph) {
      case PHI2_RISE:
          cpu->status = STATUS_INTACK;
          cpu->state_next = s_intack_T2;
          *cpu->Addr = *cpu->PC;
          *cpu->Data = cpu->status;
          Edge_hi(cpu->SYNC);
          break;
    }
}

static void s_intack_T2(Cpu8080 cpu, Cpu8080_phase ph)
{
    switch (ph) {

      case PHI2_RISE:
          Edge_lo(cpu->INTE);
          Edge_lo(cpu->SYNC);
          *cpu->Data = BUS_FLOAT;
          Edge_hi(cpu->DBIN);
          break;

      case PHI2_FALL:
          cpu->state_next = Edge_get(cpu->READY)
            ? s_intack_T3 : s_intack_TW;
          break;

    }
}

static void s_intack_TW(Cpu8080 cpu, Cpu8080_phase ph)
{
    switch (ph) {

      case PHI1_RISE:
          Edge_hi(cpu->WAIT);
          break;

      case PHI2_FALL:
          if (Edge_get(cpu->READY))
              cpu->state_next = s_intack_T3;
          break;
    }
}

static void s_intack_T3(Cpu8080 cpu, Cpu8080_phase ph)
{
    switch (ph) {

      case PHI1_RISE:
          Edge_lo(cpu->WAIT);
          break;

      case PHI2_RISE:
          *cpu->IR = *cpu->Data;
          cpu->state_next = cpu->M1T4[*cpu->IR];
          Edge_lo(cpu->DBIN);
          break;

          // For now, behavior is only defined for
          // devices presenting RST instructions.
          //
          // TODO work out how to deal with CALL.
          //
          // Bear in mind that the act of accepting
          // an interrupt disables interrupts.

    }
}
