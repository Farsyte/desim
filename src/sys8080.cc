#include "sys8080.hh"
//#include "edge.hh"

//#include <cassert>
//#include <cstdio>
//#include <cstdlib>

class Sys8080impl : public Sys8080 {
public:
    Sys8080impl(const char* name);
    virtual void  linked();
    virtual tau_t tick();
};

Sys8080impl::Sys8080impl(const char* name)
    : Sys8080(name)
{
    // Edge_RISE(&OWNED_EDGE, fn);
}

//#include "link_assert.hh"
void Sys8080impl::linked()
{
    // LINK_ASSERT(LINKED_EDGE);
    // Edge_RISE(&LINKED_EDGE, fn);
}

tau_t Sys8080impl::tick()
{
    return -1;
}

Sys8080* Sys8080::create(const char* name)
{
    return new Sys8080impl(name);
}

Sys8080::Sys8080(const char* name)
    : Module(name)
{
}

// === === === === === === === === === === === === === === === ===
//                         TESTING SUPPORT
// === === === === === === === === === === === === === === === ===

//#include "verify_elapsed_time.hh"
//#include <iostream>

// to print a value in binary:
// #include <iostream>
// #include <bitset>
//    std::cout << "cycle: BIT_PHI1 = "
//              << std::bitset<9>(BIT_PHI1) << "\n";

//#include "tau.hh"
//#include "traced.hh"
//#include "verify_elapsed_time.hh"

// static VerifyElapsedTime p1("name_of_timing_constraint", min_ns, max_ns); // which edge to which edge

void Sys8080::bist()
{
}
