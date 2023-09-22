#include "cpu8080_hlt.h"
#include <assert.h>
#include "8080_instructions.h"
#include "8080_status.h"
#include "stub.h"
#include "tau.h"

static void         s_hlt_M1T4(Cpu8080 cpu, Cpu8080_phase ph);
static void         s_hlt_M2T1(Cpu8080 cpu, Cpu8080_phase ph);
static void         s_hlt_M2T2(Cpu8080 cpu, Cpu8080_phase ph);
static void         s_hlt_M2TW(Cpu8080 cpu, Cpu8080_phase ph);

void Cpu8080_init_hlt(Cpu8080 cpu)
{
    cpu->M1T4[I8080_HLT] = s_hlt_M1T4;
}

static void s_hlt_M1T4(Cpu8080 cpu, Cpu8080_phase ph)
{
    switch (ph) {
      case PHI2_FALL:
          cpu->state_next = s_hlt_M2T1;
          break;
    }
}

static void s_hlt_M2T1(Cpu8080 cpu, Cpu8080_phase ph)
{
    switch (ph) {
      case PHI2_RISE:
          cpu->status = STATUS_HALTACK;
          cpu->state_next = s_hlt_M2T2;
          *cpu->Addr = *cpu->PC;
          *cpu->Data = cpu->status;
          Edge_hi(cpu->SYNC);
          break;
    }
}

static void s_hlt_M2T2(Cpu8080 cpu, Cpu8080_phase ph)
{
    switch (ph) {
      case PHI2_RISE:
          cpu->state_next = s_hlt_M2TW;
          Edge_lo(cpu->SYNC);
          *cpu->Addr = BUS_FLOAT;
          *cpu->Data = BUS_FLOAT;
          break;
    }
}

static void s_hlt_M2TW(Cpu8080 cpu, Cpu8080_phase ph)
{
    switch (ph) {

      case PHI1_RISE:
          Edge_hi(cpu->WAIT);
          break;

      case PHI2_RISE:
          if (Edge_get(cpu->INT) && Edge_get(cpu->INTE)) {
              Edge_hi(cpu->RETM1);
              Edge_lo(cpu->WAIT);
          }
          break;
    }
}
