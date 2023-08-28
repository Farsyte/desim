#include "cpu8080.hh"

#include <cstdio>

class Cpu8080impl : public Cpu8080 {
public:
    Cpu8080impl(const char* name);
    ~Cpu8080impl() = delete;
    virtual void linked();
};

Cpu8080impl::Cpu8080impl(const char* name)
    : Cpu8080(name)
{
    // Edge_RISE(&OWNED_EDGE, fn);
}

#include "link_assert.hh"
void Cpu8080impl::linked()
{
    LINK_ASSERT(PHI1);
    LINK_ASSERT(PHI2);
    LINK_ASSERT(RESET);
    LINK_ASSERT(READY);

    LINK_ASSERT(HOLD);
    LINK_ASSERT(INT);
}

Cpu8080* Cpu8080::create(const char* name)
{
    Cpu8080* cpu = new Cpu8080impl(name);
    fprintf(stderr, "Cpu8080 created at %p\n", (void*)cpu);
    return cpu;
}

Cpu8080::Cpu8080(const char* name)
    : Module(name)
    , WR(1)
{
}
