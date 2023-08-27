#include "cpu8080.hh"

class Cpu8080impl : public Cpu8080 {
public:
    Cpu8080impl(const char* name);
    ~Cpu8080impl() = delete;
    virtual void  linked();
    virtual tau_t tick();
};

Cpu8080impl::Cpu8080impl(const char* name)
    : Cpu8080(name)
{
    // Edge_RISE(&OWNED_EDGE, fn);
}

//#include "link_assert.hh"
void Cpu8080impl::linked()
{
    // LINK_ASSERT(LINKED_EDGE);
    // Edge_RISE(&LINKED_EDGE, fn);
}

tau_t Cpu8080impl::tick()
{
    return -1;
}

Cpu8080* Cpu8080::create(const char* name)
{
    return new Cpu8080impl(name);
}

Cpu8080::Cpu8080(const char* name)
    : Module(name)
{
}
