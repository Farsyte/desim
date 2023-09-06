#include "gen8224.h"
#include <assert.h>
#include <stdlib.h>
#include "stub.h"
#include "util.h"

unsigned            Gen8224_debug = 1;

static void         tick(Gen8224 gen);

void Gen8224_init(Gen8224 gen)
{
    assert(gen);
    assert(gen->name);
    assert(gen->name[0]);

    gen->PHI1->name = format("%s_Φ₁", gen->name);
    Edge_init(gen->PHI1);

    gen->PHI2->name = format("%s_Φ₂", gen->name);
    Edge_init(gen->PHI2);

    gen->RESET->name = format("%s_RESET", gen->name);
    Edge_init(gen->RESET);
    Edge_hi(gen->RESET);

    gen->READY->name = format("%s_READY", gen->name);
    Edge_init(gen->READY);
    Edge_lo(gen->READY);

    gen->STSTB_->name = format("%s_/STSTB", gen->name);
    Edge_init(gen->STSTB_);
    Edge_hi(gen->STSTB_);

    gen->_phase = 1;
}

void Gen8224_linked(Gen8224 gen)
{
    assert(gen->OSC);
    assert(gen->OSC->name);
    assert(gen->OSC->name[0]);

    assert(gen->RESIN_);
    assert(gen->RESIN_->name);
    assert(gen->RESIN_->name[0]);
    assert(Edge_get(gen->RESIN_) == 0);

    assert(gen->RDYIN);
    assert(gen->RDYIN->name);
    assert(gen->RDYIN->name[0]);
    assert(Edge_get(gen->RDYIN) == 0);

    assert(gen->SYNC);
    assert(gen->SYNC->name);
    assert(gen->SYNC->name[0]);
    assert(Edge_get(gen->SYNC) == 0);

    // verify required initial signal conditions

    EDGE_RISE(gen->OSC, tick, gen);
}

static void tick(Gen8224 gen)
{
    Tau                 phase = gen->_phase;

    switch (phase++) {

      case 1:
          Edge_hi(gen->STSTB_);
          Edge_hi(gen->PHI1);
          break;

      case 3:
          Edge_lo(gen->PHI1);
          Edge_hi(gen->PHI2);
          break;

      case 4:
          // While Φ₂D is not described, this is the only phase
          // where clocking these latches meets timing requirements.
          Edge_set(gen->READY, Edge_get(gen->RDYIN));
          Edge_set(gen->RESET, !Edge_get(gen->RESIN_));
          break;

      case 8:
          Edge_lo(gen->PHI2);
          break;

      case 9:
          if (Edge_get(gen->SYNC))
              Edge_lo(gen->STSTB_);
          phase = 1;
          break;
    }

    gen->_phase = phase;
}
