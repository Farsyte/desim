#include <ctype.h>
#include <stdlib.h>
#include "bist_macros.h"
#include "rtc.h"
#include "sim_types.h"
#include "tty8080.h"

static Tty8080      tty = { {"tty"} };

static Reg8         Data = { 0 };
static pEdge        WR;
static pEdge        RD;

void Tty8080_bist()
{
    PRINT_TOP();
    Tty8080_init(tty);
    tty->Data = Data;
    WR = tty->WR;
    RD = tty->RD;
    Tty8080_linked(tty);

    Tty8080_open(tty);

    Tau                 reads = 0;
    char                ch = '@';

    Tau                 idle_ns = 5000000000l;

    Tau                 t0 = rtc_ns();

    Tau                 rtc_stop = t0 + idle_ns;

    do {

        *Data = ch;
        Edge_hi(WR);
        Edge_lo(WR);
        rtc_stop = rtc_ns() + idle_ns;

        if (ch == '\r') {
            ch = '\n';
            *Data = ch;
            Edge_hi(WR);
            Edge_lo(WR);
        }

        if (ch == '\n') {
            *Data = '@';
            Edge_hi(WR);
            Edge_lo(WR);
        }

        do {
            Edge_hi(RD);
            ch = *Data;
            Edge_lo(RD);
            ++reads;
        } while (!ch && (rtc_ns() < rtc_stop));

    } while (rtc_ns() < rtc_stop);

    Tty8080_close(tty);

    t0 = rtc_ns() - t0;

    printf("ran %lu getc cycles in %lu ns: %.3f μs wall per getc\n", reads, t0, (t0 / 1000.0) / reads);

    PRINT_END();
}
