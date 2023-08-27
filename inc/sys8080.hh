#pragma once

// Microcomputer System using the Intel 8080 CPU

#include "clk8080.hh"
#include "cpu8080.hh"
#include "module.hh"

#include "edge.hh"
#include "tau.hh"

class Sys8080 : public Module {
public:
    Clk8080* clk;
    Cpu8080* cpu;

    virtual void  linked() = 0;
    virtual tau_t tick()   = 0;

    static void bist();

    static Sys8080* create(const char* name);

protected:
    Sys8080(const char*); // please use Sys8080::create(name)
};
