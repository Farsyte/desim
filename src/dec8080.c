#include "dec8080.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "stub.h"
#include "util.h"

#define ASSERT_LINK(which, reqval)                         \
    do {                                                   \
        if (!dec->which)                                   \
            FAIL(" %s: '%s' is not linked",              \
                 dec->name, #which);                       \
        if (!dec->which->name)                             \
            FAIL(" %s: '%s' has no name",             \
                 dec->name, #which);                       \
        if (!dec->which->name[0])                          \
            FAIL(" %s: '%s' has empty name",          \
                 dec->name, #which);                       \
        if (Edge_get(dec->which) != reqval)                \
            FAIL(" %s: '%s' has wrong initial state", \
                 dec->name, #which);                       \
    } while (0)

static void         assert_optional_link(Dec8080 dec, Edge e, Cstr list, int num);

static void         decode_rd(Dec8080 dec);
static void         decode_wr(Dec8080 dec);

static void         release(Dec8080 dec);

static void         shadow_enable(Dec8080 dec);
static void         shadow_disable(Dec8080 dec);

void Dec8080_init(Dec8080 dec)
{
    assert(dec);
    assert(dec->name);
    assert(dec->name[0]);

    dec->asserted = 0;
    dec->shadowed = 0;
}

void Dec8080_linked(Dec8080 dec)
{
    ASSERT_LINK(MEMR_, 1);
    ASSERT_LINK(MEMW_, 1);
    ASSERT_LINK(IOR_, 1);
    ASSERT_LINK(IOW_, 1);

    ASSERT_LINK(DBIN, 0);
    ASSERT_LINK(WR_, 1);

    if (dec->SHADOW_ENA || dec->SHADOW_CE) {
        ASSERT_LINK(SHADOW_ENA, 0);
        ASSERT_LINK(SHADOW_CE, 0);
    }

    assert(!dec->asserted);
    assert(!dec->asserted);
    assert(!dec->shadowed);
    assert(!dec->shadowed);

    for (int page = 0; page < DEC8080_PAGES; ++page)
        assert_optional_link(dec, dec->memr[page], "memr", page);
    for (int page = 0; page < DEC8080_PAGES; ++page)
        assert_optional_link(dec, dec->memw[page], "memw", page);

    for (int port = 0; port < DEC8080_PORTS; ++port)
        assert_optional_link(dec, dec->devr[port], "devr", port);
    for (int port = 0; port < DEC8080_PORTS; ++port)
        assert_optional_link(dec, dec->devw[port], "devw", port);

    if (dec->SHADOW_ENA) {
        EDGE_RISE(dec->SHADOW_ENA, shadow_enable, dec);
        EDGE_FALL(dec->SHADOW_ENA, shadow_disable, dec);
    }

    EDGE_RISE(dec->DBIN, decode_rd, dec);
    EDGE_FALL(dec->WR_, decode_wr, dec);
    EDGE_RISE(dec->WR_, release, dec);
    EDGE_FALL(dec->DBIN, release, dec);
}

static void assert_optional_link(Dec8080 dec, Edge e, Cstr list, int num)
{
    if (e) {
        if (!e->name)
            FAIL(" %s: link '%s[%d]' has no name", dec->name, list, num);
        if (!e->name[0])
            FAIL(" %s: link '%s[%d]' has empty name", dec->name, list, num);
        if (Edge_get(e) != 0)
            FAIL(" %s: link '%s[%d]' is high, must be low", dec->name, list, num);
    }
}

static void decode_rd(Dec8080 dec)
{
    if (Edge_get(dec->MEMR_) == 0) {
        Word                p = *dec->Addr >> 10;
        pEdge               e = dec->memr[p];
        if (e) {
            Edge_hi(e);
            dec->asserted = e;
        }
    }
    if (Edge_get(dec->IOR_) == 0) {
        Word                p = *dec->Addr & 0377;
        pEdge               e = dec->devr[p];
        if (e) {
            Edge_hi(e);
            dec->asserted = e;
        }
    }
}

static void decode_wr(Dec8080 dec)
{
    if (Edge_get(dec->MEMW_) == 0) {
        Word                p = *dec->Addr >> 10;
        pEdge               e = dec->memw[p];
        if (e) {
            Edge_hi(e);
            dec->asserted = e;
        }
    }
    if (Edge_get(dec->IOW_) == 0) {
        Word                p = *dec->Addr & 0377;
        pEdge               e = dec->devw[p];
        if (e) {
            Edge_hi(e);
            dec->asserted = e;
        }
    }
}

static void release(Dec8080 dec)
{
    if (dec->asserted) {
        Edge_lo(dec->asserted);
        dec->asserted = 0;
    }
    if (dec->asserted) {
        Edge_lo(dec->asserted);
        dec->asserted = 0;
    }
}

static void shadow_enable(Dec8080 dec)
{
    if (dec->SHADOW_CE && !dec->shadowed && (dec->memr[0] != dec->SHADOW_CE)) {
        dec->shadowed = dec->memr[0];
        dec->memr[0] = dec->SHADOW_CE;
    }
    if (dec->SHADOW_CE && !dec->shadowed && (dec->memr[0] != dec->SHADOW_CE)) {
        dec->shadowed = dec->memr[0];
        dec->memr[0] = dec->SHADOW_CE;
    }
}

static void shadow_disable(Dec8080 dec)
{
    if (dec->SHADOW_CE && (dec->memr[0] == dec->SHADOW_CE)) {
        dec->memr[0] = dec->shadowed;
        dec->shadowed = 0;
    }
    if (dec->SHADOW_CE && (dec->memr[0] == dec->SHADOW_CE)) {
        dec->memr[0] = dec->shadowed;
        dec->shadowed = 0;
    }
}
