#include "dec8080.h"

#include <stdio.h>
#include <assert.h>

#include "bist_macros.h"
#include "stub.h"

static Reg16        Addr;


static Edge         MEMR_ = { {"bist.MEMR_"} };
static Edge         MEMW_ = { {"bist.MEMW_"} };
static Edge         IOR_ = { {"bist.IOR_"} };
static Edge         IOW_ = { {"bist.IOW_"} };

#define	PAGE_000		000000
#define	PAGE_001		002000
#define	PAGE_007		016000

static Edge         SHADOW_ENA = { {"bist.SENA"} };

static Edge         RAM000 = { {"bist.RAM000"} };
static Edge         RAM001 = { {"bist.RAM001"} };
static Edge         ROM007 = { {"bist.ROM007"} };

static Edge         IDEV000 = { {"bist.IDEV000"} };
static Edge         IDEV001 = { {"bist.IDEV001"} };
static Edge         ODEV000 = { {"bist.ODEV000"} };
static Edge         ODEV001 = { {"bist.ODEV001"} };

static Dec8080      dec =
  { {"bist.dec1", Addr, MEMR_, MEMW_, IOR_, IOW_, SHADOW_ENA, ROM007,
     {RAM000, RAM001, 0, 0, 0, 0, 0, ROM007},
     {IDEV000, IDEV001}, {ODEV000, ODEV001}}
};

void Dec8080_bist()
{
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
