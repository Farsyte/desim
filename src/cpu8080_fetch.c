#include "cpu8080_fetch.h"

#include "8080_status.h"

static void         s_fetch_T1(Cpu8080 cpu, Cpu8080_phase ph);
static void         s_fetch_T2_incpc(Cpu8080 cpu, Cpu8080_phase ph);
static void         s_fetch_TW_wait(Cpu8080 cpu, Cpu8080_phase ph);
static void         s_fetch_T3_tmpir(Cpu8080 cpu, Cpu8080_phase ph);

void Cpu8080_init_fetch(Cpu8080 cpu)
{
    cpu->fetch = s_fetch_T1;
}

static void s_fetch_T1(Cpu8080 cpu, Cpu8080_phase ph)
{
    switch (ph) {
      case PHI2_RISE:
          cpu->status = STATUS_FETCH;
          cpu->state_next = s_fetch_T2_incpc;
          *cpu->Addr = *cpu->PC;
          *cpu->Data = cpu->status;
          Edge_hi(cpu->SYNC);
          break;
    }
}

static void s_fetch_T2_incpc(Cpu8080 cpu, Cpu8080_phase ph)
{
    switch (ph) {

      case PHI2_RISE:
          Edge_lo(cpu->SYNC);
          *cpu->PC = *cpu->PC + 1;
          *cpu->Data = BUS_FLOAT;
          Edge_hi(cpu->DBIN);
          break;

      case PHI2_FALL:
          cpu->state_next = Edge_get(cpu->READY)
            ? s_fetch_T3_tmpir : s_fetch_TW_wait;
          break;

    }
}

static void s_fetch_TW_wait(Cpu8080 cpu, Cpu8080_phase ph)
{
    switch (ph) {

      case PHI1_RISE:
          Edge_hi(cpu->WAIT);
          break;

      case PHI2_FALL:
          if (Edge_get(cpu->READY))
              cpu->state_next = s_fetch_T3_tmpir;
          break;
    }
}

static void s_fetch_T3_tmpir(Cpu8080 cpu, Cpu8080_phase ph)
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
    }
}
