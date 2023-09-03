#include "cpu8080.h"

#include <assert.h>

#include "8080_instructions.h"
#include "8080_status.h"
#include "cpu8080_fetch.h"
#include "cpu8080_halt.h"
#include "cpu8080_invalid.h"
#include "cpu8080_m1t1.h"
#include "cpu8080_nop.h"
#include "cpu8080_reset.h"
#include "edge.h"
#include "stub.h"
#include "util.h"

unsigned            Cpu8080_debug = 2;

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

static void Cpu8080_init_decode(Cpu8080 cpu)
{
    Cpu8080_init_invalid(cpu);
    Cpu8080_init_fetch(cpu);
    Cpu8080_init_nop(cpu);
    Cpu8080_init_halt(cpu);

    Cpu8080_init_M1T1(cpu);
}

void Cpu8080_init(Cpu8080 cpu)
{

    assert(cpu);
    assert(cpu->name);
    assert(cpu->name[0]);

    cpu->INTE->name = format("%s.INTE", cpu->name);
    Edge_init(cpu->INTE);

    cpu->DBIN->name = format("%s.DBIN", cpu->name);
    Edge_init(cpu->DBIN);

    cpu->WR_->name = format("%s.WR_", cpu->name);
    Edge_init(cpu->WR_);

    cpu->SYNC->name = format("%s.SYNC", cpu->name);
    Edge_init(cpu->SYNC);

    cpu->WAIT->name = format("%s.WAIT", cpu->name);
    Edge_init(cpu->WAIT);

    cpu->HLDA->name = format("%s.HLDA", cpu->name);
    Edge_init(cpu->HLDA);

    Cpu8080_init_decode(cpu);
    Cpu8080_init_reset(cpu);
}
