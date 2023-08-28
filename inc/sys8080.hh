#pragma once

// Microcomputer System using the Intel 8080 CPU

#include "clk8080.hh"
#include "cpu8080.hh"
#include "module.hh"

#include "edge.hh"
#include "tau.hh"

class Sys8080 : public Module {
public:
    static void bist();

    static Sys8080* create(const char* name);

protected:
    Sys8080(const char*); // please use Sys8080::create(name)
};
