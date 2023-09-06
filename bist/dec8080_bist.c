#include <assert.h>
#include <stdio.h>
#include "bist_macros.h"
#include "dec8080.h"
#include "stub.h"

static Reg16        Addr;

static Edge         MEMR_ = { {"bist_MEMR_"} };
static Edge         MEMW_ = { {"bist_MEMW_"} };
static Edge         IOR_ = { {"bist_IOR_"} };
static Edge         IOW_ = { {"bist_IOW_"} };

#define	PAGE_000		000000
#define	PAGE_001		002000
#define	PAGE_007		016000

static Edge         SHADOW_ENA = { {"bist_SENA"} };

static Edge         RAM000 = { {"bist_RAM000"} };
static Edge         RAM001 = { {"bist_RAM001"} };
static Edge         ROM007 = { {"bist_ROM007"} };

static Edge         IDEV000 = { {"bist_IDEV000"} };
static Edge         IDEV001 = { {"bist_IDEV001"} };
static Edge         ODEV000 = { {"bist_ODEV000"} };
static Edge         ODEV001 = { {"bist_ODEV001"} };

static Dec8080      dec = { {"bist_dec"} };

void Dec8080_bist()
{
    Edge_init(MEMR_);
    Edge_init(MEMW_);
    Edge_init(IOR_);
    Edge_init(IOW_);
    Edge_init(SHADOW_ENA);

    Edge_hi(MEMR_);
    Edge_hi(MEMW_);
    Edge_hi(IOR_);
    Edge_hi(IOW_);

    Edge_init(RAM000);
    Edge_init(RAM001);
    Edge_init(ROM007);
    Edge_init(IDEV000);
    Edge_init(IDEV001);
    Edge_init(ODEV000);
    Edge_init(ODEV001);

    dec->name = "bist_dec1";
    Dec8080_init(dec);

    dec->Addr = Addr;
    dec->MEMR_ = MEMR_;
    dec->MEMW_ = MEMW_;
    dec->IOR_ = IOR_;
    dec->IOW_ = IOW_;
    dec->SHADOW_ENA = SHADOW_ENA;
    dec->SHADOW_SEL = ROM007;

    dec->page[000] = RAM000;
    dec->page[001] = RAM001;
    dec->page[007] = ROM007;

    dec->devi[000] = IDEV000;
    dec->devi[001] = IDEV001;

    dec->devo[000] = ODEV000;
    dec->devo[001] = ODEV001;

    Dec8080_linked(dec);

    PRINT_TOP();

    Edge_init(MEMR_);
    Edge_hi(MEMR_);

    Edge_init(MEMW_);
    Edge_hi(MEMW_);

    Edge_init(IOR_);
    Edge_hi(IOR_);

    Edge_init(IOW_);
    Edge_hi(IOW_);

    Edge_init(RAM000);
    Edge_lo(RAM000);

    Edge_init(RAM001);
    Edge_lo(RAM001);

    Dec8080_init(dec);
    Dec8080_linked(dec);

    Dec8080_take(dec, 007, ROM007);

    *Addr = PAGE_007;

    Edge_lo(MEMR_);
    assert(Edge_get(ROM007));
    Edge_hi(MEMR_);
    assert(!Edge_get(ROM007));

    *Addr = PAGE_000;

    Edge_hi(SHADOW_ENA);
    Edge_lo(MEMR_);
    assert(Edge_get(ROM007));
    assert(!Edge_get(RAM000));
    Edge_hi(MEMR_);
    assert(!Edge_get(ROM007));

    Edge_lo(SHADOW_ENA);
    Edge_lo(MEMR_);
    assert(Edge_get(RAM000));
    assert(!Edge_get(ROM007));
    Edge_hi(MEMR_);
    assert(!Edge_get(RAM000));

    *Addr = PAGE_001;

    Edge_hi(SHADOW_ENA);

    Edge_lo(MEMR_);
    assert(Edge_get(RAM001));
    assert(!Edge_get(ROM007));
    Edge_hi(MEMR_);
    assert(!Edge_get(RAM001));

    Edge_lo(SHADOW_ENA);

    Edge_lo(MEMR_);
    assert(Edge_get(RAM001));
    assert(!Edge_get(ROM007));
    Edge_hi(MEMR_);
    assert(!Edge_get(RAM001));

    *Addr = PAGE_007;

    Edge_lo(MEMW_);
    assert(Edge_get(ROM007));
    Edge_hi(MEMW_);
    assert(!Edge_get(ROM007));

    *Addr = PAGE_000;

    Edge_hi(SHADOW_ENA);
    Edge_lo(MEMW_);
    assert(Edge_get(ROM007));
    assert(!Edge_get(RAM000));
    Edge_hi(MEMW_);
    assert(!Edge_get(ROM007));

    Edge_lo(SHADOW_ENA);
    Edge_lo(MEMW_);
    assert(Edge_get(RAM000));
    assert(!Edge_get(ROM007));
    Edge_hi(MEMW_);
    assert(!Edge_get(RAM000));

    *Addr = PAGE_001;

    Edge_hi(SHADOW_ENA);

    Edge_lo(MEMW_);
    assert(Edge_get(RAM001));
    assert(!Edge_get(ROM007));
    Edge_hi(MEMW_);
    assert(!Edge_get(RAM001));

    Edge_lo(SHADOW_ENA);

    Edge_lo(MEMW_);
    assert(Edge_get(RAM001));
    assert(!Edge_get(ROM007));
    Edge_hi(MEMW_);
    assert(!Edge_get(RAM001));

    *Addr = 0;

    assert(!Edge_get(IDEV000));
    assert(!Edge_get(IDEV001));
    assert(!Edge_get(ODEV000));
    assert(!Edge_get(ODEV001));
    Edge_lo(IOR_);
    assert(Edge_get(IDEV000));
    Edge_hi(IOR_);
    assert(!Edge_get(IDEV000));

    *Addr = 1;

    assert(!Edge_get(IDEV000));
    assert(!Edge_get(IDEV001));
    assert(!Edge_get(ODEV000));
    assert(!Edge_get(ODEV001));
    Edge_lo(IOR_);
    assert(Edge_get(IDEV001));
    Edge_hi(IOR_);
    assert(!Edge_get(IDEV001));

    *Addr = 0;

    assert(!Edge_get(IDEV000));
    assert(!Edge_get(IDEV001));
    assert(!Edge_get(ODEV000));
    assert(!Edge_get(ODEV001));
    Edge_lo(IOW_);
    assert(Edge_get(ODEV000));
    Edge_hi(IOW_);
    assert(!Edge_get(ODEV000));

    *Addr = 1;

    assert(!Edge_get(IDEV000));
    assert(!Edge_get(IDEV001));
    assert(!Edge_get(ODEV000));
    assert(!Edge_get(ODEV001));
    Edge_lo(IOW_);
    assert(Edge_get(ODEV001));
    Edge_hi(IOW_);
    assert(!Edge_get(ODEV001));

    PRINT_END();
}
