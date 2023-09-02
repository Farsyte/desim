#include "ctl8228.h"


#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "clock.h"
#include "stub.h"
#include "rtc.h"

#include "traced.h"

#include "8080_status.h"

#include "bist_macros.h"

static Byte         Data;

static Edge         STSTB_ = { {"STSTB_"} };
static Edge         DBIN = { {"DBIN"} };
static Edge         WR_ = { {"WR_"} };
static Edge         HLDA = { {"HLDA"} };

static Ctl8228      ctl = { {"ctl1", &Data, STSTB_, DBIN, WR_, HLDA} };

static Traced       tSTSTB_ = { {"STSTB_"} };
static Traced       tDBIN = { {"DBIN"} };
static Traced       tWR_ = { {"WR_"} };
static Traced       tHLDA = { {"HLDA"} };

static Traced       tBUSEN_ = { {"BUSEN_"} };

static Traced       tIOW_ = { {"IOW_"} };
static Traced       tMEMW_ = { {"MEMW_"} };
static Traced       tIOR_ = { {"IOR_"} };
static Traced       tMEMR_ = { {"MEMR_"} };
static Traced       tINTA_ = { {"INTA_"} };

static pTraced      trace_list[] = {

    tSTSTB_,
    tDBIN,
    tWR_,

    tBUSEN_,

    tIOW_,
    tMEMW_,
    tIOR_,
    tMEMR_,
    tINTA_,

    tHLDA,
};

static size_t       trace_count =
  sizeof trace_list / sizeof trace_list[0];

static void dostatus(Byte status)
{
    unsigned            sWO_ = status & STATUS_WO;

    // Run through eight T-states for each status,
    // each needing nine clocks. This results in
    // one page of plot per status.

    for (unsigned t = 0; t < 8; ++t) {
        for (unsigned p = 0; p < 9; ++p) {

            // wiggle STSTB_, DBIN, WR_, and HLDA
            // trying to get a response.

            // First four t-states are a normal read or write.
            // Second four are cut short by HLDA if reading.

            switch (t * 10 + p) {
              default:
                  break;
              case 4:
                  *ctl->Data = status;
                  break;
              case 8:
                  Edge_lo(ctl->STSTB_);
                  break;
              case 10:
                  Edge_hi(ctl->STSTB_);
                  break;
              case 14:
                  if (sWO_)
                      Edge_hi(ctl->DBIN);
                  break;
              case 21:
                  if (!sWO_)
                      Edge_lo(ctl->WR_);
                  break;
              case 24:
                  if (sWO_)
                      Edge_lo(ctl->DBIN);
                  break;
              case 31:
                  if (!sWO_)
                      Edge_hi(ctl->WR_);
                  break;
              case 44:
                  *ctl->Data = status;
                  break;
              case 48:
                  Edge_lo(ctl->STSTB_);
                  break;
              case 50:
                  Edge_hi(ctl->STSTB_);
                  break;
              case 54:
                  if (sWO_)
                      Edge_hi(ctl->DBIN);
                  break;
              case 57:
                  if (sWO_)
                      Edge_hi(ctl->HLDA);
                  break;
              case 61:
                  if (!sWO_)
                      Edge_lo(ctl->WR_);
                  break;
              case 64:
                  if (sWO_)
                      Edge_lo(ctl->DBIN);
                  break;
              case 67:
                  if (sWO_)
                      Edge_lo(ctl->HLDA);
                  break;
              case 71:
                  if (!sWO_)
                      Edge_hi(ctl->WR_);
                  break;
            }

            // advance another 55.556 ns
            Clock_cycle();
        }
    }
}

void Ctl8228_bist()
{
    PRINT_TOP();

    Edge_hi(STSTB_);
    Edge_lo(DBIN);
    Edge_hi(WR_);
    Edge_lo(HLDA);

    Clock_init(1000, 18);
    assert(TAU == 0);
    assert(UNIT == 0);
    Ctl8228_init(ctl);
    Ctl8228_linked(ctl);

    Traced_init(tSTSTB_, ctl->STSTB_, 1);
    Traced_init(tDBIN, ctl->DBIN, 0);
    Traced_init(tWR_, ctl->WR_, 1);
    Traced_init(tHLDA, ctl->HLDA, 0);

    Traced_init(tIOW_, ctl->IOW_, 1);
    Traced_init(tMEMW_, ctl->MEMW_, 1);
    Traced_init(tIOR_, ctl->IOR_, 1);
    Traced_init(tMEMR_, ctl->MEMR_, 1);
    Traced_init(tINTA_, ctl->INTA_, 1);

    Tau                 umin = TAU;

    // exercise the signals. more complete testing will hapen when
    // hooked up to an 8224 and 8080. For now, I just have to
    // synthesize the input signals (and Data).

    dostatus(STATUS_FETCH);     // 0b10100010      // (1) MEMR M1
    dostatus(STATUS_MREAD);     // 0b10000010      // (2) MEMR
    dostatus(STATUS_MWRITE);    // 0b00000000      // (3)              /WO
    dostatus(STATUS_SREAD);     // 0b10000110      // (4) MEMR STACK
    dostatus(STATUS_SWRITE);    // 0b00000100      // (5)      STACK   /WO
    dostatus(STATUS_INPUTRD);   // 0b01000010      // (6) INP
    dostatus(STATUS_OUTPUTWR);  // 0b00010000      // (7) OUT          /WO
    dostatus(STATUS_INTACK);    // 0b00100011      // (8)      M1        INTA
    dostatus(STATUS_HALTACK);   // 0b10001010      // (9) MEMR    HLTA
    dostatus(STATUS_INTACKW);   // 0b00101011      // (10)     M1 HLTA   INTA

    printf("\n");
    printf("Signal Traces:\n");
    for (size_t i = 0; i < trace_count; ++i)
        Traced_update(trace_list[i]);

    double              us_per_unit = (TAU * 0.001) / UNIT;

    Tau                 maxm = 72;
    Tau                 u = umin;
    Tau                 umax = UNIT;
    while (u < umax) {
        Tau                 hi = u + maxm;
        if (hi > umax)
            hi = umax;
        printf("\n");
        printf("From %lu to %lu u:\n", u, hi);
        printf("From %.3f to %.3f μs:\n",
               us_per_unit * u, us_per_unit * hi);
        for (size_t i = 0; i < trace_count; ++i)
            Traced_print(trace_list[i], u, hi - u);
        u = hi;
    }
    PRINT_END();
}
