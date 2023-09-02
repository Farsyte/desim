#include "ctl8228.h"
#include <assert.h>
#include <stdlib.h>

#include "edge.h"

#include "stub.h"
#include "util.h"

#include "8080_status.h"

unsigned            Ctl8228_debug = 1;

void Ctl8228_init(Ctl8228 ctl)
{
    assert(ctl);
    assert(ctl->name);
    assert(ctl->name[0]);

    ctl->MEMR_->name = format("%s./MEMR", ctl->name);
    Edge_init(ctl->MEMR_);
    Edge_hi(ctl->MEMR_);

    ctl->MEMW_->name = format("%s./MEMW", ctl->name);
    Edge_init(ctl->MEMW_);
    Edge_hi(ctl->MEMW_);

    ctl->IOR_->name = format("%s./IOR", ctl->name);
    Edge_init(ctl->IOR_);
    Edge_hi(ctl->IOR_);

    ctl->IOW_->name = format("%s./IOW", ctl->name);
    Edge_init(ctl->IOW_);
    Edge_hi(ctl->IOW_);

    ctl->INTA_->name = format("%s./INTA", ctl->name);
    Edge_init(ctl->INTA_);
    Edge_hi(ctl->INTA_);
}

static void wr_fall(Ctl8228 ctl)
{
    if (ctl->status & STATUS_OUT)
        Edge_lo(ctl->IOW_);
    else
        Edge_lo(ctl->MEMW_);
}
static void wr_rise(Ctl8228 ctl)
{
    if (ctl->status & STATUS_OUT)
        Edge_hi(ctl->IOW_);
    else
        Edge_hi(ctl->MEMW_);
}
static void hlda_rise(Ctl8228 ctl)
{
    Byte                STATUS = ctl->status;
    if (STATUS & STATUS_MEMR)
        Edge_hi(ctl->MEMR_);
    if (STATUS & STATUS_INP)
        Edge_hi(ctl->IOR_);
    if (STATUS & STATUS_INTA)
        Edge_hi(ctl->INTA_);
}
static void dbin_fall(Ctl8228 ctl)
{
    Byte                STATUS = ctl->status;
    if (STATUS & STATUS_MEMR)
        Edge_hi(ctl->MEMR_);
    if (STATUS & STATUS_INP)
        Edge_hi(ctl->IOR_);
    if (STATUS & STATUS_INTA)
        Edge_hi(ctl->INTA_);
}
static void ststb_fall(Ctl8228 ctl)
{
    Byte                STATUS = *ctl->Data;
    ctl->status = STATUS;
    if ((STATUS & STATUS_MEMR) && !(STATUS & STATUS_HLTA))
        Edge_lo(ctl->MEMR_);
    if (STATUS & STATUS_INP)
        Edge_lo(ctl->IOR_);
    if (STATUS & STATUS_INTA)
        Edge_lo(ctl->INTA_);
}

void Ctl8228_linked(Ctl8228 ctl)
{
    assert(ctl->Data);

    assert(ctl->STSTB_);
    assert(ctl->STSTB_->name);
    assert(ctl->STSTB_->name[0]);

    assert(ctl->DBIN);
    assert(ctl->DBIN->name);
    assert(ctl->DBIN->name[0]);

    assert(ctl->WR_);
    assert(ctl->WR_->name);
    assert(ctl->WR_->name[0]);

    assert(ctl->HLDA);
    assert(ctl->HLDA->name);
    assert(ctl->HLDA->name[0]);

    // verify required initial signal conditions

    assert(Edge_get(ctl->STSTB_));
    assert(!Edge_get(ctl->DBIN));
    assert(Edge_get(ctl->WR_));
    assert(!Edge_get(ctl->HLDA));

    EDGE_FALL(ctl->STSTB_, ststb_fall, ctl);
    EDGE_FALL(ctl->WR_, wr_fall, ctl);
    EDGE_RISE(ctl->WR_, wr_rise, ctl);
    EDGE_FALL(ctl->DBIN, dbin_fall, ctl);
    EDGE_RISE(ctl->HLDA, hlda_rise, ctl);
}
