#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "bist_macros.h"
#include "rtc.h"
#include "sio8080.h"
#include "stub.h"

static Sio8080      sio = { {"bist.sio"} };
static Reg8         Data;

void Sio8080_bist()
{
    PRINT_TOP();

    Sio8080_init(sio);
    sio->Data = Data;
    Sio8080_linked(sio);

    time_t              now = time(0);
    time_t              end = now + 10.0;
    while (time(0) < end) {
        char                buf[4096];
        int                 rs;
        int                 ws;
        Tau                 t0;

        t0 = rtc_ns();
        rs = read(sio->rx, buf, sizeof buf);
        t0 = rtc_ns() - t0;
        if (rs < 0) {
            if (errno == EWOULDBLOCK)
                rs = 0;
            else
                FERR("read(%s)", sio->rxname);
        }
        if (rs == 0) {
            if (t0 > 10000)
                STUB("read(%s) %.3fμs", sio->rxname, t0 / 1000.0);
            continue;
        }
        if (t0 > 0)
            STUB("read(%s) %.3fμs", sio->rxname, t0 / 1000.0);
        t0 = rtc_ns();
        ws = write(sio->tx, buf, rs);
        t0 = rtc_ns() - t0;
        if (ws < 0) {
            if (t0 > 10000000)
                STUB("write(%s) %.3fμs", sio->rxname, t0 / 1000.0);
            FERR("write(%s)", sio->txname);
        }
        if (ws != rs)
            FAIL("wrote %d of %d bytes to %s", ws, rs, sio->txname);
        if (t0 > 0)
            STUB("write(%s) %.3fμs", sio->rxname, t0 / 1000.0);
        STUB("copied %d bytes", ws);
    }

    Sio8080_down(sio);

    PRINT_END();
    abort();
}
