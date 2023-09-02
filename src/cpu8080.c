#include "cpu8080.h"

#include <assert.h>

#include "8080_status.h"
#include "cpu8080_reset.h"
#include "edge.h"
#include "stub.h"
#include "util.h"

unsigned            Cpu8080_debug = 2;

static int          state_tables_init_done = 0;
static Cpu8080_state *m1t4[0400];
static Cpu8080_state *m2t1[0400];

static void s_invalid_reported(Cpu8080 cpu, Cpu8080_phase ph)
{
    (void)cpu;
    (void)ph;
}

static void s_invalid(Cpu8080 cpu, Cpu8080_phase ph)
{
    (void)cpu;
    (void)ph;

    switch (ph) {
      default:
          break;
      case PHI1_RISE:
          break;
      case PHI2_RISE:
          break;
      case PHI2_FALL:
          printf("%s reached invalid state\n", cpu->name);
          printf("  Addr: 0%o6o\n", *cpu->Addr);
          printf("  Data: 0%o6o\n", *cpu->Data);
          printf("  Inst: 0%o6o\n", *cpu->IR);

          cpu->state_next = s_invalid_reported;
          break;
    }
}

static void s_noop(Cpu8080 cpu, Cpu8080_phase ph)
{
    (void)cpu;
    (void)ph;

    switch (ph) {
      default:
          break;
      case PHI1_RISE:
          break;
      case PHI2_RISE:
          break;
      case PHI2_FALL:
          cpu->state_next = m2t1[*cpu->IR];
          break;
    }
}

static void s_template(Cpu8080 cpu, Cpu8080_phase ph)
{
    (void)cpu;
    (void)ph;

    switch (ph) {
      default:
          break;
      case PHI1_RISE:
          break;
      case PHI2_RISE:
          break;
      case PHI2_FALL:
          cpu->state_next = s_template;
          break;
    }
}

static void s_tmpir(Cpu8080 cpu, Cpu8080_phase ph)
{
    switch (ph) {
      default:
          break;
      case PHI1_RISE:
          break;
      case PHI2_RISE:
          *cpu->IR = *cpu->Data;
          Edge_lo(cpu->DBIN);
          break;
      case PHI2_FALL:
          cpu->state_next = m1t4[*cpu->IR];
          break;
    }
}

static void s_fetch_wait(Cpu8080 cpu, Cpu8080_phase ph)
{
    switch (ph) {
      default:
          break;
      case PHI1_RISE:
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
          *cpu->Data = 0x00;    // TODO reprsent FLOATING
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

/** s_fetch(cpu, ph):
 */
static void s_fetch(Cpu8080 cpu, Cpu8080_phase ph)
{
    switch (ph) {
      default:
          break;
      case PHI1_RISE:
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

static void phi1_rise(Cpu8080 cpu)
{
    cpu->state = cpu->state_next;
    cpu->state(cpu, PHI1_RISE);
}

static void phi2_rise(Cpu8080 cpu)
{
    cpu->state(cpu, PHI2_RISE);
}

static void phi2_fall(Cpu8080 cpu)
{
    cpu->state(cpu, PHI2_FALL);
}

void Cpu8080_linked(Cpu8080 cpu)
{
    assert(cpu->PHI1);
    assert(cpu->PHI1->name);
    assert(cpu->PHI1->name[0]);
    assert(Edge_get(cpu->PHI1) == 0);

    assert(cpu->PHI2);
    assert(cpu->PHI2->name);
    assert(cpu->PHI2->name[0]);
    assert(Edge_get(cpu->PHI2) == 0);

    assert(cpu->RESET);
    assert(cpu->RESET->name);
    assert(cpu->RESET->name[0]);
    assert(Edge_get(cpu->RESET) == 1);

    assert(cpu->READY);
    assert(cpu->READY->name);
    assert(cpu->READY->name[0]);
    assert(Edge_get(cpu->READY) == 0);

    assert(cpu->HOLD);
    assert(cpu->HOLD->name);
    assert(cpu->HOLD->name[0]);
    assert(Edge_get(cpu->HOLD) == 0);

    assert(cpu->INT);
    assert(cpu->INT->name);
    assert(cpu->INT->name[0]);
    assert(Edge_get(cpu->INT) == 0);

    EDGE_RISE(cpu->PHI1, phi1_rise, cpu);
    EDGE_RISE(cpu->PHI2, phi2_rise, cpu);
    EDGE_FALL(cpu->PHI2, phi2_fall, cpu);
}

static void init_decode()
{
    if (state_tables_init_done)
        return;

    m1t4[0000] = s_template;
    m2t1[0000] = s_template;

    for (unsigned b = 0; b <= 0377; ++b) {
        m1t4[b] = s_invalid;
        m2t1[b] = s_fetch;
    }

    m1t4[0000] = s_noop;

    state_tables_init_done = 1;
}

void Cpu8080_init(Cpu8080 cpu)
{
    init_decode();

    assert(cpu);
    assert(cpu->name);
    assert(cpu->name[0]);

    cpu->INTE->name = format("%s.INTE", cpu->name);
    Edge_init(cpu->INTE);

    cpu->DBIN->name = format("%s.DBIN", cpu->name);
    Edge_init(cpu->DBIN);

    cpu->WR_->name = format("%s.WR_", cpu->name);
    Edge_init(cpu->WR_);
    Edge_hi(cpu->WR_);

    cpu->SYNC->name = format("%s.SYNC", cpu->name);
    Edge_init(cpu->SYNC);

    cpu->WAIT->name = format("%s.WAIT", cpu->name);
    Edge_init(cpu->WAIT);

    cpu->HLDA->name = format("%s.HLDA", cpu->name);
    Edge_init(cpu->HLDA);

    Cpu8080_init_reset(cpu, s_fetch);
}
