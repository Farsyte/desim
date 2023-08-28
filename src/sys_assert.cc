#include "sys8080.hh"

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
    Sys8080& sys = *(Sys8080::create("sys1"));
    sys.linked();
}
