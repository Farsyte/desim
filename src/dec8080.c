#include "dec8080.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "util.h"
#include "stub.h"

pEdge Dec8080_take(Dec8080 dec, int which_page, Edge e)
{
    assert(dec);
    assert(0 <= which_page);
    assert(which_page < DEC8080_PAGES);
    pEdge               rv = dec->page[which_page];
    dec->page[which_page] = e;
    return rv;
}

void Dec8080_init(Dec8080 dec)
{
    Edge_init(dec->SHADOW_ENA);
    Edge_init(dec->SHADOW_SEL);
    dec->hidden_ram = 0;
    dec->enabled = 0;
}

#define ASSERT_LINK(which, reqval)                         \
    do {                                                   \
        if (!dec->which)                                   \
            STUB(" %s: link '%s' is not set",              \
                 dec->name, #which);                       \
        else if (!dec->which->name)                        \
            STUB(" %s: link '%s' has no name",            \
                 dec->name, #which);                       \
        else if (!dec->which->name[0])                     \
            STUB(" %s: link '%s' has empty name",         \
                 dec->name, #which);                       \
        else if (Edge_get(dec->which) != reqval)           \
            STUB(" %s: link '%s' has wrong initial state", \
                 dec->name, #which);                       \
        else                                               \
            break;                                         \
        abort();                                           \
    } while (0)

static void assert_optional_link(Dec8080 dec, Edge e, Cstr list,
                                 int num)
{
    if (e) {
        if (!e->name)
            STUB(" %s: link '%s[%d]' has no which",
                 dec->name, list, num);
        else if (!e->name[0])
            STUB(" %s: link '%s[%d]' has empty which",
                 dec->name, list, num);
        else if (Edge_get(e) != 0)
            STUB(" %s: link '%s[%d]' is high, must be low",
                 dec->name, list, num);
        else
            return;
        abort();
    }
}

static void shadow_is_rom(Dec8080 dec)
{
    if (!dec->SHADOW_SEL)
        return;

    if (dec->hidden_ram)
        return;

    if (dec->SHADOW_SEL == dec->page[0])
        return;

    dec->hidden_ram = dec->page[0];
    dec->page[0] = dec->SHADOW_SEL;
}

static void shadow_is_ram(Dec8080 dec)
{
    if (!dec->SHADOW_SEL)
        return;

    if (dec->SHADOW_SEL != dec->page[0])
        return;

    dec->page[0] = dec->hidden_ram;
    dec->hidden_ram = 0;
}

static void decode_mem(Dec8080 dec)
{
    Word                p = *dec->Addr >> 10;
    pEdge               e = dec->page[p];
    if (!e)
        return;
    Edge_hi(e);
    dec->enabled = e;
}

static void decode_ior(Dec8080 dec)
{
    Word                p = *dec->Addr & 0377;
    pEdge               e = dec->devi[p];
    if (!e)
        return;
    Edge_hi(e);
    dec->enabled = e;
}

static void decode_iow(Dec8080 dec)
{
    Word                p = *dec->Addr & 0377;
    pEdge               e = dec->devo[p];
    if (!e)
        return;
    Edge_hi(e);
    dec->enabled = e;
}

static void release_enable(Dec8080 dec)
{
    if (!dec->enabled)
        return;
    Edge_lo(dec->enabled);
    dec->enabled = 0;
}

void Dec8080_linked(Dec8080 dec)
{
    ASSERT_LINK(MEMR_, 1);
    ASSERT_LINK(MEMW_, 1);
    ASSERT_LINK(IOR_, 1);
    ASSERT_LINK(IOW_, 1);

    if (dec->SHADOW_SEL || dec->SHADOW_ENA) {
        ASSERT_LINK(SHADOW_SEL, 0);
        ASSERT_LINK(SHADOW_ENA, 0);
    }

    assert(!dec->hidden_ram);
    assert(!dec->enabled);

    for (int page = 0; page < DEC8080_PAGES; ++page)
        assert_optional_link(dec, dec->page[page], "page", page);
    for (int port = 0; port < DEC8080_PORTS; ++port)
        assert_optional_link(dec, dec->devi[port], "devi", port);
    for (int port = 0; port < DEC8080_PORTS; ++port)
        assert_optional_link(dec, dec->devo[port], "devo", port);

    if (dec->SHADOW_ENA) {
        EDGE_RISE(dec->SHADOW_ENA, shadow_is_rom, dec);
        EDGE_FALL(dec->SHADOW_ENA, shadow_is_ram, dec);
    }

    EDGE_FALL(dec->MEMR_, decode_mem, dec);
    EDGE_FALL(dec->MEMW_, decode_mem, dec);
    EDGE_FALL(dec->IOR_, decode_ior, dec);
    EDGE_FALL(dec->IOW_, decode_iow, dec);

    EDGE_RISE(dec->MEMR_, release_enable, dec);
    EDGE_RISE(dec->MEMW_, release_enable, dec);
    EDGE_RISE(dec->IOR_, release_enable, dec);
    EDGE_RISE(dec->IOW_, release_enable, dec);

}
