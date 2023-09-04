#include <assert.h>
#include <stdio.h>

#include "8080_instructions.h"
#include "8080_status.h"
#include "bist_macros.h"
#include "clock.h"
#include "cpu8080.h"
#include "ctl8228.h"
#include "gen8224.h"
#include "rtc.h"
#include "traced.h"

extern Gen8224      gen;
extern Ctl8228      ctl;
extern Cpu8080      cpu;

// Signals owned by the environment, so far:
Edge                RESIN_ = { {"bist./RESIN"} };
Edge                RDYIN = { {"bist.RDYIN"} };
Edge                HOLD = { {"bist.HOLD"} };
Edge                INT = { {"bist.INT"} };

static const pEdge  PHI1 = gen->PHI1;
static const pEdge  PHI2 = gen->PHI2;
static const pEdge  RESET = gen->RESET;
static const pEdge  READY = gen->READY;
static const pEdge  STSTB_ = gen->STSTB_;

static const pEdge  MEMR_ = ctl->MEMR_;
static const pEdge  MEMW_ = ctl->MEMW_;
static const pEdge  IOR_ = ctl->IOR_;
static const pEdge  IOW_ = ctl->IOW_;
static const pEdge  INTA_ = ctl->INTA_;

// Addr and Data are "const pointer"
static const pReg16 Addr = cpu->Addr;
static const pReg8  Data = cpu->Data;

static const pEdge  INTE = cpu->INTE;
static const pEdge  DBIN = cpu->DBIN;
static const pEdge  WR_ = cpu->WR_;
static const pEdge  SYNC = cpu->SYNC;
static const pEdge  WAIT = cpu->WAIT;
static const pEdge  HLDA = cpu->HLDA;
static const pEdge  RETM1 = cpu->RETM1;
static const pEdge  INT_RST = cpu->INT_RST;

static Traced       tPHI1 = { {"PHI1"} };
static Traced       tPHI2 = { {"PHI2"} };
static Traced       tSYNC = { {"SYNC"} };
static Traced       tSTSTB_ = { {"STSTB_"} };

static Traced       tRESIN_ = { {"RESIN_"} };
static Traced       tRESET = { {"RESET"} };

static Traced       tRDYIN = { {"RDYIN"} };
static Traced       tREADY = { {"READY"} };

static Traced       tDBIN = { {"DBIN"} };
static Traced       tWR_ = { {"WR_"} };
static Traced       tMEMR_ = { {"MEMR_"} };
static Traced       tMEMW_ = { {"MEMW_"} };
static Traced       tIOR_ = { {"IOR_"} };
static Traced       tIOW_ = { {"IOW_"} };

static Traced       tINTE = { {"INTE"} };
static Traced       tINT = { {"INT"} };
static Traced       tINTA_ = { {"INTA_"} };

static Traced       tHOLD = { {"HOLD"} };
static Traced       tHLDA = { {"HLDA"} };

static Traced       tWAIT = { {"WAIT"} };
static Traced       tRETM1 = { {"RETM1"} };
static Traced       tINT_RST = { {"INT_RST"} };

static pTraced      trace_list[] = {
    tPHI1,
    tPHI2,
    tSYNC,
    tSTSTB_,
    tRESIN_,
    tRESET,
    tRDYIN,
    tREADY,
    tDBIN,
    tWR_,
    tMEMR_,
    tMEMW_,
    tIOR_,
    tIOW_,
    tINTE,
    tINT,
    tINTA_,
    tHOLD,
    tHLDA,
    tWAIT,
    tRETM1,
    tINT_RST,
};
static size_t       trace_count =
  sizeof trace_list / sizeof trace_list[0];

Gen8224             gen = {
    {"bist.gen1", CLOCK, RESIN_, RDYIN, SYNC
     // PHI1 PHI2 RESET READY STSTB_
     }
};
Ctl8228             ctl = {
    {"bist.ctl1", Data, STSTB_, DBIN, WR_, HLDA
     // MEMR_ MEMW_ IOR_ IOW_ INTA_
     }
};
Cpu8080             cpu = {
    {"bist.cpu1", PHI1, PHI2, RESET, READY, HOLD, INT
     // Addr Data INTE DBIN WR_ SYNC WAIT HLDA
     }
};

unsigned            fp_verbose = 5;
unsigned            fp_output = 0;

static void fp_verbose_adj()
{
    if (fp_verbose < 1)
        return;
    fp_output++;
    if (fp_output < 1000)
        return;
    fp_output = 0;
    fp_verbose--;
}

static void fp_memr_fall(void *arg)
{
    if (fp_verbose < 9)
        return;
    (void)arg;
    printf("%8.3f: /MEMR↓ 0%06o\n", TU, *Addr);
    fp_verbose_adj();
}
static void fp_memr_rise(void *arg)
{
    if (fp_verbose < 9)
        return;
    (void)arg;
    printf("%8.3f: /MEMR↑ 0%06o 0%03o\n", TU, *Addr, *Data);
    fp_verbose_adj();
}
static void fp_ststb_fall(void *arg)
{
    if (fp_verbose < 2)
        return;

    (void)arg;

    Byte                status = *Data;
    printf("%8.3f: /STSTB↓ 0%03o", TU, status);
    printf("%s%s%s%s%s%s%s%s",
           (status & STATUS_INTA) ? " INTA" : "",
           (status & STATUS_WO) ? "" : " /WO",
           (status & STATUS_STACK) ? " STACK" : "",
           (status & STATUS_HLTA) ? " HLTA" : "",
           (status & STATUS_OUT) ? " OUT" : "",
           (status & STATUS_M1) ? " M1" : "",
           (status & STATUS_INP) ? " INP" : "",
           (status & STATUS_MEMR) ? " MEMR" : "");
    printf("%s%s%s%s%s%s%s%s%s%s",
           (status == STATUS_FETCH) ? " == STATUS_FETCH" : "",
           (status == STATUS_MREAD) ? " == STATUS_MREAD" : "",
           (status == STATUS_MWRITE) ? " == STATUS_MWRITE" : "",
           (status == STATUS_SREAD) ? " == STATUS_SREAD" : "",
           (status == STATUS_SWRITE) ? " == STATUS_SWRITE" : "",
           (status == STATUS_INPUTRD) ? " == STATUS_INPUTRD" : "",
           (status == STATUS_OUTPUTWR) ? " == STATUS_OUTPUTWR" : "",
           (status == STATUS_INTACK) ? " == STATUS_INTACK" : "",
           (status == STATUS_HALTACK) ? " == STATUS_HALTACK" : "",
           (status == STATUS_INTACKW) ? " == STATUS_INTACKW" : "");
    printf("\n");
    fp_verbose_adj();
}
static void fp_dbin_rise(void *arg)
{
    if (fp_verbose < 3)
        return;
    (void)arg;
    printf("%8.3f: DBIN↑%s%s 0%06o\n", TU,
           Edge_get(MEMR_) ? "" : " /MEMR",
           Edge_get(IOR_) ? "" : " /IOR", *Addr);
    fp_verbose_adj();
}
static void fp_dbin_fall(void *arg)
{
    if (fp_verbose < 1)
        return;
    (void)arg;
    // MEMR_ and IOR_ are released on DBIN falling edge,
    // service happening before we get to this service.
    printf("%8.3f: DBIN↓ 0%06o 0%03o %s\n",
           TU, *Addr, *Data, disassemble(*Data));
    fp_verbose_adj();
}

static void fp_print_setup()
{
    EDGE_FALL(MEMR_, fp_memr_fall, 0);
    EDGE_RISE(MEMR_, fp_memr_rise, 0);
    EDGE_FALL(STSTB_, fp_ststb_fall, 0);
    EDGE_RISE(DBIN, fp_dbin_rise, 0);
    EDGE_FALL(DBIN, fp_dbin_fall, 0);
}

static void Cpu8080_bist_init()
{
    Edge_lo(RESIN_);
    Edge_lo(RDYIN);
    Edge_lo(HOLD);
    Edge_lo(INT);

    Clock_init(18.00);

    Gen8224_init(gen);
    Ctl8228_init(ctl);
    Cpu8080_init(cpu);

    Gen8224_linked(gen);
    Ctl8228_linked(ctl);
    Cpu8080_linked(cpu);
}

static void sync_counter(Tau *ctr)
{
    ++*ctr;
}

void Cpu8080_bench()
{
    PRINT_TOP();

    Cpu8080_bist_init();

    // Initial state: RESET is asserted, READY is not.

    Clock_cycle_by(9 * 6);

    // Release RESET.
    Edge_hi(RESIN_);
    Clock_cycle_by(9 * 7);

    // Assert READY
    Edge_hi(RDYIN);
    Clock_cycle_by(9 * 11);

    Tau                 delta_unit = 1000;
    Tau                 wall_t0_ns, wall_dt_ns;
    double              sim_t0_us = 0;
    double              sim_dt_us = 0;
    Tau                 mint = 25000000;

    Tau                 sync_count = 0;
    EDGE_RISE(cpu->SYNC, sync_counter, &sync_count);

    while (1) {
        sync_count = 0;

        sim_t0_us = TU;
        wall_t0_ns = rtc_ns();
        Clock_cycle_by(delta_unit);
        wall_dt_ns = rtc_ns() - wall_t0_ns;
        sim_dt_us = TU - sim_t0_us;

        if (wall_dt_ns >= mint)
            break;

        if (wall_dt_ns <= mint / 10) {
            delta_unit *= 10;
        } else {
            delta_unit = (delta_unit * mint * 2.0) / wall_dt_ns;
        }
    }

    double              w_ms = wall_dt_ns / 1000000.0;
    double              s_ms = sim_dt_us / 1000.0;

    fprintf(stderr, "\n");
    fprintf(stderr, "Cpu8080 benchmark:\n");
    fprintf(stderr, " wall time per SYNC: %.3f μs\n",
            w_ms * 1000.0 / sync_count);
    fprintf(stderr, "  sim time per SYNC: %.3f μs\n",
            s_ms * 1000.0 / sync_count);
    fprintf(stderr, "  wall time elapsed: %.3f ms\n", w_ms);
    fprintf(stderr, "   sim time elapsed: %.3f ms\n", s_ms);

    double              time_ratio = s_ms / w_ms;
    fprintf(stderr,
            "  sim running at %.2fx real time"
            " (higher is better)\n", time_ratio);
    fprintf(stderr, "\n");

    // I am simulating the 8080 at several times its original
    // speed, configured to use a 18.00 MHz clock.
    //
    // Compiled with -g -O0, sim runs at about 4x real time.
    // Compiled with -g -O2, sim runs at about 7x real time.
    // Compiled with -g -O3, sim runs at about 9x real time.
    //
    // Compiled with -g -pg -O0, sim runs at about 1x real time.
    //
    // Fail this test if the ratio falls under 0.5
    //
    assert(time_ratio > 0.5);

    PRINT_END();
}

void Cpu8080_bist()
{
    PRINT_TOP();

    Cpu8080_bist_init();

    printf("\n");

    fp_print_setup();

    // Signals not imported by any module:
    //   WAIT INTE Addr INTA_ IOW_ IOR_ MEMW_ MEMR_

    (void)Addr;         // TODO log content of Addr bus
    (void)Data;         // TODO log content of Data bus

    Traced_init(tPHI1, PHI1, 0);
    Traced_init(tPHI2, PHI2, 0);
    Traced_init(tSYNC, SYNC, 0);
    Traced_init(tSTSTB_, STSTB_, 1);
    Traced_init(tRESIN_, RESIN_, 1);
    Traced_init(tRESET, RESET, 0);
    Traced_init(tRDYIN, RDYIN, 0);
    Traced_init(tREADY, READY, 0);
    Traced_init(tDBIN, DBIN, 0);
    Traced_init(tWR_, WR_, 1);
    Traced_init(tMEMR_, MEMR_, 1);
    Traced_init(tMEMW_, MEMW_, 1);
    Traced_init(tIOR_, IOR_, 1);
    Traced_init(tIOW_, IOW_, 1);
    Traced_init(tINTE, INTE, 0);
    Traced_init(tINT, INT, 0);
    Traced_init(tINTA_, INTA_, 1);
    Traced_init(tHOLD, HOLD, 0);
    Traced_init(tHLDA, HLDA, 0);
    Traced_init(tWAIT, WAIT, 0);
    Traced_init(tRETM1, RETM1, 0);
    Traced_init(tINT_RST, INT_RST, 0);

    Traced_active_boring(tRDYIN);
    Traced_active_boring(tREADY);

    Tau                 umin = UNIT;

    fprintf(stderr, "\nStarting cycles in Cpu8080_bist\n");

    // Initial state: RESET is asserted, READY is not.

    Clock_cycle_by(9 * 6);

    // Release RESET.
    Edge_hi(RESIN_);
    Clock_cycle_by(9 * 7);

    // Assert READY
    Edge_hi(RDYIN);
    Clock_cycle_by(9 * 7);

    while (!Edge_get(cpu->SYNC))
        Clock_cycle();

    // Execute an EI instruction

    while (!Edge_get(cpu->DBIN))
        Clock_cycle();
    while (Edge_get(ctl->MEMR_))
        Clock_cycle();
    *cpu->Data = I8080_EI;

    while (!Edge_get(cpu->SYNC))
        Clock_cycle();

    // Execute a HLT instruction

    while (!Edge_get(cpu->DBIN))
        Clock_cycle();
    while (Edge_get(ctl->MEMR_))
        Clock_cycle();
    *cpu->Data = I8080_HLT;

    while (!Edge_get(cpu->WAIT))
        Clock_cycle();

    // brief delay before using reset
    Clock_cycle_by(9 * 10 - 1);

    // Exercise RESET from HALT state

    Edge_lo(RESIN_);
    Edge_lo(RDYIN);
    Clock_cycle_by(9 * 6);
    Edge_hi(RESIN_);
    Clock_cycle_to(9 * 7);
    Edge_hi(RDYIN);
    Clock_cycle_to(9 * 11);

    while (!Edge_get(cpu->SYNC))
        Clock_cycle();

    // Execute a DI instruction
    while (!Edge_get(cpu->DBIN))
        Clock_cycle();
    while (Edge_get(ctl->MEMR_))
        Clock_cycle();

    *cpu->Data = I8080_DI;

    while (!Edge_get(cpu->SYNC))
        Clock_cycle();

    // Execute several NOP instructions
    for (int i = 0; i < 4; ++i) {
        while (!Edge_get(cpu->DBIN))
            Clock_cycle();
        while (Edge_get(ctl->MEMR_))
            Clock_cycle();
        *cpu->Data = I8080_NOP;
        while (!Edge_get(cpu->SYNC))
            Clock_cycle();
    }
    Clock_cycle_by(9 * 4 - 3);

    printf("\n");
    printf("Signal Traces:\n");
    for (size_t i = 0; i < trace_count; ++i)
        Traced_update(trace_list[i]);

    double              us_per_unit = TU / UNIT;

    Tau                 maxm = 72;
    Tau                 u = umin;
    Tau                 umax = UNIT;
    while (u < umax) {
        Tau                 hi = u + maxm;
        if (hi > umax)
            hi = umax;
        printf("\n");
        printf("From %.3f to %.3f μs:\n",
               us_per_unit * u, us_per_unit * hi);
        for (size_t i = 0; i < trace_count; ++i)
            Traced_print(trace_list[i], u, hi - u);
        u = hi;
    }

    PRINT_END();
}
