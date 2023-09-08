#include "tty8080.h"
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#include "rtc.h"
#include "stub.h"
#include "util.h"

static int          tty_israw = 0;

static void         tty_rd(Tty8080 tty);
static void         tty_wr(Tty8080 tty);

void Tty8080_init(Tty8080 tty)
{
    assert(tty);
    assert(tty->name);
    assert(tty->name[0]);

    tty->WR->name = format("%s.WR");
    Edge_init(tty->WR);
    Edge_lo(tty->WR);

    tty->RD->name = format("%s.RD");
    Edge_init(tty->RD);
    Edge_lo(tty->RD);
}

void Tty8080_linked(Tty8080 tty)
{
    assert(tty->Data);

    EDGE_RISE(tty->WR, tty_wr, tty);
    EDGE_RISE(tty->RD, tty_rd, tty);

}

void Tty8080_open(Tty8080 tty)
{
    int                 rv;

    (void)tty;

    if (!tty_israw) {
        rv = system("stty raw");
        if (rv < 0)
            perror("stty raw");
        tty_israw = 1;
    }
}

void Tty8080_close(Tty8080 tty)
{
    int                 rv;

    (void)tty;

    if (tty_israw) {
        rv = system("stty cooked");
        if (rv < 0)
            perror("stty cooked");
        tty_israw = 0;
    }
}

static void tty_wr(Tty8080 tty)
{
    int                 rv;
    // is there any way we can deliver the output to memory
    // to be output by a separate thread?
    rv = write(1, tty->Data, 1);
    if (rv > 0)
        return;
    if (rv == 0)
        fprintf(stderr, "tty_wr: write returns zero");
    else
        perror("tty_wr: write");
    abort();
}

static void tty_rd(Tty8080 tty)
{
    int                 rv;
    // is there any way we can have the data delivered to us
    // by a separate thread, so we can service this by just
    // looking at local memory?
    rv = read(0, tty->Data, 1);
    if (rv > 0)
        return;
    *tty->Data = 0x00;
    if (rv == 0)
        return;
    if ((rv < 0) && (errno == EAGAIN))
        return;
    perror("tty_rd: read");
    abort();
}
