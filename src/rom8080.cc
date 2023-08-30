#include "rom8080.hh"
//#include "edge.hh"
//#include "util.hh"

#include "8080_status.hh"
#include "link_assert.hh"

#include <cassert>
#include <cstdio>
//#include <cstdlib>

class Rom8080impl : public Rom8080 {
public:
    Rom8080impl(const char* name);
    virtual void linked();

    // TODO: get ROM contents from a .hex file
    // into a collecton of Rom8080 objects.

protected:
    void rd();
    Byte data[1024];
};

Rom8080* Rom8080::create(const char* name)
{
    return new Rom8080impl(name);
}

Rom8080::Rom8080(const char* name)
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

Rom8080impl::Rom8080impl(const char* name)
    : Rom8080(name)
{
}

// instruction fetch:
// - Addr is stable soon after SYNC↑
// - MEMR is active from /STSTB↓ until DBIN↑
// - /RDYIN is sampled by Clk at Φ₁↑ into /READY
// - /READY is sampled by Cpu at Φ₂↓; if active, insert TW
// - Data must be valid at Φ₂↓

void Rom8080impl::linked()
{
    LINK_ASSERT(Addr);
    LINK_ASSERT(Data);

    LINK_ASSERT(RDYIN);
    LINK_ASSERT(DBIN);

    Edge_RISE(DBIN, rd);
}

void Rom8080impl::rd()
{
    if (!ENABLE.get())
        return;
    *Data = data[*Addr & 01777];
}
