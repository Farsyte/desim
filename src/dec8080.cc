#include "dec8080.hh"

#include <cassert>
#include <cstddef>
#include <cstdlib>

#include "stub.hh"

class Dec8080impl : public Dec8080 {
public:
    Dec8080impl(const char* name);
    virtual void linked();

protected:
    void  decode_memr();
    void  decode_memw();
    Edge* rom_enabled;
    Edge* ram_enabled;
    void  release_mem();

    void  decode_dev();
    Edge* dev_enabled;
    void  release_dev();
};

Dec8080* Dec8080::create(const char* name)
{
    return new Dec8080impl(name);
}

Dec8080::Dec8080(const char* name)
    : Module(name)
{
}

Dec8080impl::Dec8080impl(const char* name)
    : Dec8080(name)
    , rom_enabled(0)
    , dev_enabled(0)
{
}

#include "link_assert.hh"

void Dec8080impl::linked()
{
    LINK_ASSERT(Addr);

    LINK_ASSERT(MEMR);
    LINK_ASSERT(MEMW);
    LINK_ASSERT(IOR);
    LINK_ASSERT(IOW);

    Edge_FALL(MEMR, decode_memr);
    Edge_FALL(MEMW, decode_memw);
    Edge_FALL(IOR, decode_dev);
    Edge_FALL(IOW, decode_dev);

    Edge_RISE(MEMR, release_mem);
    Edge_RISE(MEMW, release_mem);
    Edge_RISE(IOR, release_dev);
    Edge_RISE(IOW, release_dev);
}

void Dec8080impl::decode_memr()
{
    assert(!rom_enabled);
    assert(!dev_enabled);

    Word addr = *Addr;

    rom_enabled = ROM[077 & (addr >> 10)];
    if (rom_enabled) {
        rom_enabled->hi();
        return;
    }

    ram_enabled = RAM[017 & (addr >> 12)];
    if (ram_enabled) {
        ram_enabled->hi();
        return;
    }

    STUB(" no ROM or RAM mapped at 0%05o\n", addr);
    abort();
}

void Dec8080impl::decode_memw()
{
    assert(!rom_enabled);
    assert(!dev_enabled);

    Word addr = *Addr;

    ram_enabled = RAM[017 & (addr >> 12)];
    if (ram_enabled) {
        ram_enabled->hi();
        return;
    }

    STUB(" no RAM mapped at 0%05o\n", addr);
    abort();
}

void Dec8080impl::decode_dev()
{
    assert(!rom_enabled);
    assert(!dev_enabled);

    unsigned addr = *Addr;
    unsigned dev  = addr & 0377;

    dev_enabled = DEV[255 & dev];

    if (dev_enabled) {
        dev_enabled->hi();
        return;
    }

    STUB(" no device mapped at 0%03o\n", dev);
    abort();
}

void Dec8080impl::release_mem()
{
    if (ram_enabled) {
        ram_enabled->lo();
        ram_enabled = NULL;
    }
    if (rom_enabled) {
        rom_enabled->lo();
        rom_enabled = NULL;
    }
}

void Dec8080impl::release_dev()
{
    if (dev_enabled) {
        dev_enabled->lo();
        dev_enabled = NULL;
    }
}
