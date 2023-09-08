#include <assert.h>
#include "8080_instructions.h"
#include "8080_status.h"
#include "bist_macros.h"
#include "clock.h"
#include "edge.h"
#include "rtc.h"
#include "stub.h"
#include "sys8080.h"
#include "traced.h"
#include "util.h"

static void         Sys8080_bist_init();
static void         Sys8080_bist_down();

static void         fp_verbose_adj();
static void         fp_memr_fall(void *arg);
static void         fp_memr_rise(void *arg);
static void         fp_ststb_fall(void *arg);
static void         fp_dbin_rise(void *arg);
static void         fp_dbin_fall(void *arg);
static void         fp_print_setup();
static void         sync_counter(Tau *ctr);

static Sys8080      sys = { {"sys"} };

static const pEdge  RESIN_ = sys->RESIN_;
static const pEdge  RDYIN = sys->RDYIN;
static const pEdge  HOLD = sys->HOLD;
static const pEdge  INT = sys->INT;

static const pEdge  PHI1 = sys->gen->PHI1;
static const pEdge  PHI2 = sys->gen->PHI2;

static const pEdge  RESET = sys->gen->RESET;
static const pEdge  READY = sys->gen->READY;
static const pEdge  STSTB_ = sys->gen->STSTB_;

static const pEdge  MEMR_ = sys->ctl->MEMR_;
static const pEdge  MEMW_ = sys->ctl->MEMW_;
static const pEdge  IOR_ = sys->ctl->IOR_;
static const pEdge  IOW_ = sys->ctl->IOW_;
static const pEdge  INTA_ = sys->ctl->INTA_;

static const pReg16 Addr = sys->cpu->Addr;
static const pReg8  Data = sys->cpu->Data;

static const pEdge  INTE = sys->cpu->INTE;
static const pEdge  DBIN = sys->cpu->DBIN;
static const pEdge  WR_ = sys->cpu->WR_;
static const pEdge  SYNC = sys->cpu->SYNC;
static const pEdge  WAIT = sys->cpu->WAIT;
static const pEdge  HLDA = sys->cpu->HLDA;

static const pEdge  RETM1 = sys->cpu->RETM1;

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
};

static size_t       trace_count = sizeof trace_list / sizeof trace_list[0];

static unsigned     fp_verbose = 5;
static unsigned     fp_output = 0;

void Sys8080_bench()
{
    PRINT_TOP();

    Sys8080_bist_init();

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
    Tau                 mint = 2500000;

    Tau                 sync_count = 0;
    EDGE_RISE(SYNC, sync_counter, &sync_count);

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
    fprintf(stderr, "Sys8080 benchmark:\n");
    fprintf(stderr, " wall time per SYNC: %.3f μs\n", w_ms * 1000.0 / sync_count);
    fprintf(stderr, "  sim time per SYNC: %.3f μs\n", s_ms * 1000.0 / sync_count);
    fprintf(stderr, "  wall time elapsed: %.3f ms\n", w_ms);
    fprintf(stderr, "   sim time elapsed: %.3f ms\n", s_ms);

    double              time_ratio = s_ms / w_ms;
    fprintf(stderr, "  sim running at %.2fx real time" " (higher is better)\n", time_ratio);
    fprintf(stderr, "\n");

    assert(time_ratio > 0.5);

    Sys8080_bist_down();

    PRINT_END();
}

void Sys8080_bist()
{
    PRINT_TOP();

    Sys8080_bist_init();

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

    Traced_active_boring(tRDYIN);
    Traced_active_boring(tREADY);

    Tau                 umin = UNIT;

    fprintf(stderr, "\nStarting cycles in Sys8080_bist\n");

    // === === === === === === === === === === === === === ===
    // Desired Test Program:
    // - some time in RESET
    // - some time not yet READY
    // - RESET runs code from SHADOW ROM
    // - immediate jump to normal ROM location
    // - turn off the shadow facility
    // - load a program into RAM
    // - execute the RAM program
    // === === === === === === === === === === === === === ===

    // Currently, I have 8 KiB of ROM and 16 KiB of RAM simulated.
    // I want to boot into FIG-FORTH 8080, which (unless I change
    // the .asm file) wants to be in low ram.
    //
    // So I need a very tiny bit of code in ROM that shadows
    // the start of memory, which jumps up to the actual ROM
    // location, does some init, and dives into FORTH.
    //
    // FORTH wants to see some CP/M BIOS entries. These need
    // to land in ROM.

    // Initial state: RESET is asserted, READY is not.

    Clock_cycle_by(9 * 6);

    // Release RESET.
    Edge_hi(RESIN_);
    Clock_cycle_by(9 * 7);

    // Assert READY
    Edge_hi(RDYIN);
    Clock_cycle_by(9 * 7);

    // Run a few instructions
    Clock_cycle_by(9 * 4 * 3);

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
        printf("From %.3f to %.3f μs:\n", us_per_unit * u, us_per_unit * hi);
        for (size_t i = 0; i < trace_count; ++i)
            Traced_print(trace_list[i], u, hi - u);
        u = hi;
    }

    Sys8080_bist_down();

    PRINT_END();
}

static void Sys8080_bist_init()
{
    Clock_init(18.00);
    Sys8080_init(sys);
    sys->OSC = CLOCK;
    Sys8080_linked(sys);
}

static void Sys8080_bist_down()
{
    Sys8080_down(sys);
}

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
           (status & STATUS_INTA) ? " INTA" : "", (status & STATUS_WO) ? "" : " /WO",
           (status & STATUS_STACK) ? " STACK" : "", (status & STATUS_HLTA) ? " HLTA" : "",
           (status & STATUS_OUT) ? " OUT" : "", (status & STATUS_M1) ? " M1" : "", (status & STATUS_INP) ? " INP" : "",
           (status & STATUS_MEMR) ? " MEMR" : "");

    printf("%s%s%s%s%s%s%s%s%s%s",
           (status == STATUS_FETCH) ? " == STATUS_FETCH" : "", (status == STATUS_MREAD) ? " == STATUS_MREAD" : "",
           (status == STATUS_MWRITE) ? " == STATUS_MWRITE" : "", (status == STATUS_SREAD) ? " == STATUS_SREAD" : "",
           (status == STATUS_SWRITE) ? " == STATUS_SWRITE" : "", (status == STATUS_INPUTRD) ? " == STATUS_INPUTRD" : "",
           (status == STATUS_OUTPUTWR) ? " == STATUS_OUTPUTWR" : "", (status == STATUS_INTACK) ? " == STATUS_INTACK" : "",
           (status == STATUS_HALTACK) ? " == STATUS_HALTACK" : "", (status == STATUS_INTACKW) ? " == STATUS_INTACKW" : "");

    printf("\n");
    fp_verbose_adj();
}
static void fp_dbin_rise(void *arg)
{
    if (fp_verbose < 3)
        return;
    (void)arg;
    printf("%8.3f: DBIN↑%s%s 0%06o\n", TU, Edge_get(MEMR_) ? "" : " /MEMR", Edge_get(IOR_) ? "" : " /IOR", *Addr);
    fp_verbose_adj();
}
static void fp_dbin_fall(void *arg)
{
    if (fp_verbose < 1)
        return;
    (void)arg;
    // MEMR_ and IOR_ are released on DBIN falling edge,
    // service happening before we get to this service.
    printf("%8.3f: DBIN↓ 0%06o 0%03o %s\n", TU, *Addr, *Data, disassemble(*Data));
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

static void sync_counter(Tau *ctr)
{
    ++*ctr;
}
