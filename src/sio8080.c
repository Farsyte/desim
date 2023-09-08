#include "sio8080.h"
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#include "stub.h"
#include "util.h"

static void         sio_rx_init(Sio8080 sio);
static void         sio_tx_init(Sio8080 sio);
static void         sio_rx_down(Sio8080 sio);
static void         sio_tx_down(Sio8080 sio);
static int          sio_fifo(Cstr chipname);

void Sio8080_init(Sio8080 sio)
{
    assert(sio);
    assert(sio->name);
    assert(sio->name[0]);

    sio->RD->name = format("%s.RD", sio->name);
    Edge_init(sio->RD);
    Edge_lo(sio->RD);

    sio->WR->name = format("%s.WR", sio->name);
    Edge_init(sio->WR);
    Edge_lo(sio->WR);

}

void Sio8080_linked(Sio8080 sio)
{
    assert(sio->Data);

    sio_rx_init(sio);
    sio_tx_init(sio);
}

void Sio8080_down(Sio8080 sio)
{
    sio_rx_down(sio);
    sio_tx_down(sio);
}

static void sio_rx_init(Sio8080 sio)
{
    sio->rxprod = sio->rxcons = 0;
    sio->rxname = format("/tmp/desim.%s.rx", sio->name);
    sio->rx = sio_fifo(sio->rxname);
}

static void sio_tx_init(Sio8080 sio)
{
    sio->txprod = sio->txcons = 0;
    sio->txname = format("/tmp/desim.%s.tx", sio->name);
    sio->tx = sio_fifo(sio->txname);
}

static void sio_rx_down(Sio8080 sio)
{
    if (sio->rx >= 0) {
        STUB("closing fd %d, %s", sio->rx, sio->rxname);
        close(sio->rx);
        sio->rx = -1;
    }
}

static void sio_tx_down(Sio8080 sio)
{
    if (sio->tx >= 0) {
        STUB("closing fd %d, %s", sio->tx, sio->txname);
        close(sio->tx);
        sio->tx = -1;
    }
}

static int sio_fifo(Cstr pathname)
{
    int                 fd;
    int                 rv;

    fd = open(pathname, O_RDWR | O_NONBLOCK);
    if (fd < 0) {
        if (errno == ENOENT) {
            rv = mkfifo(pathname, 0640);
            if (rv < 0)
                FERR("mkfifo(%s)", pathname);
            fd = open(pathname, O_RDWR | O_NONBLOCK);
        }
        if (fd < 0)
            FERR("open(%s)", pathname);
    }
    STUB("opened fd %d, %s", fd, pathname);
    return fd;

}
