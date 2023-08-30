#include "ram8080.hh"
//#include "edge.hh"
//#include "util.hh"

#include "8080_status.hh"
#include "link_assert.hh"

#include <cassert>
#include <cstdio>
//#include <cstdlib>

class Ram8080impl : public Ram8080 {
public:
    Ram8080impl(const char* name);
    virtual void linked();

    // TODO: get RAM contents fram a .hex file
    // into a collecton of Ram8080 objects.

protected:
    void rd();
    void wr();
    Byte data[4096];
};

Ram8080* Ram8080::create(const char* name)
{
    return new Ram8080impl(name);
}

Ram8080::Ram8080(const char* name)
    : Module(name)
{
}

// Initialize the implementation of the
// Clock Generation for the Intel 8080A CPU
//
// This is based on Figure 3-3 of the MCS
// extended to include the RESET signal
// to match the Intel 8224 capability.
//
// Generally, we can use this module to latch
// any incoming signals that the CPU wants to
// have change shortly after Φ₂ rises.

Ram8080impl::Ram8080impl(const char* name)
    : Ram8080(name)
{
}

// instruction fetch:
// - Addr is stable soon after SYNC↑
// - MEMR is active fram /STSTB↓ until DBIN↑
// - /RDYIN is sampled by Clk at Φ₁↑ into /READY
// - /READY is sampled by Cpu at Φ₂↓; if active, insert TW
// - Data must be valid at Φ₂↓

void Ram8080impl::linked()
{
    LINK_ASSERT(Addr);
    LINK_ASSERT(Data);

    LINK_ASSERT(RDYIN);
    LINK_ASSERT(DBIN);
    LINK_ASSERT(WR);

    Edge_RISE(DBIN, rd);
    Edge_FALL(DBIN, wr);
}

void Ram8080impl::rd()
{
    if (!ENABLE.get())
        return;
    *Data = data[*Addr & 07777];
}

void Ram8080impl::wr()
{
    if (!ENABLE.get())
        return;
    data[*Addr & 07777] = *Data;
}
