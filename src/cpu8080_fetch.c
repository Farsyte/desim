#include "cpu8080_fetch.h"

#include "8080_status.h"

static void s_tmpir(Cpu8080 cpu, Cpu8080_phase ph)
{
    switch (ph) {
      default:
          break;
      case PHI1_RISE:
          Edge_lo(cpu->WAIT);
          break;
      case PHI2_RISE:
          *cpu->IR = *cpu->Data;
          Edge_lo(cpu->DBIN);
          break;
      case PHI2_FALL:
          cpu->state_next = cpu->M1T4[*cpu->IR];
          break;
    }
}

static void s_fetch_wait(Cpu8080 cpu, Cpu8080_phase ph)
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
          if (Edge_get(cpu->READY)) {
              cpu->state_next = s_tmpir;
          } else {
              cpu->state_next = s_fetch_wait;
          }
          break;
    }
}

static void s_incpc(Cpu8080 cpu, Cpu8080_phase ph)
{
    switch (ph) {
      default:
          break;
      case PHI1_RISE:
          break;
      case PHI2_RISE:
          Edge_lo(cpu->SYNC);
          *cpu->PC = *cpu->PC + 1;
          *cpu->Data = 0x00;    // TODO represent FLOATING
          Edge_hi(cpu->DBIN);
          break;
      case PHI2_FALL:
          if (Edge_get(cpu->READY)) {
              cpu->state_next = s_tmpir;
          } else {
              cpu->state_next = s_fetch_wait;
          }
          break;
    }
}

static void s_fetch(Cpu8080 cpu, Cpu8080_phase ph)
{
    switch (ph) {
      default:
          break;
      case PHI1_RISE:
          Edge_lo(cpu->RETM1);
          break;
      case PHI2_RISE:
          *cpu->Addr = *cpu->PC;
          *cpu->Data = STATUS_FETCH;
          Edge_hi(cpu->SYNC);
          break;
      case PHI2_FALL:
          cpu->state_next = s_incpc;
          break;
    }
}

void Cpu8080_init_fetch(Cpu8080 cpu)
{
    cpu->fetch = s_fetch;
}
