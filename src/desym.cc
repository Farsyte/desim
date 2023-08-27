// starting point for execution of desym

#include "actions.hh"
#include "edge.hh"
#include "clk8080.hh"

int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;

    // TODO process the command line parameters
    // TODO dispatch to appropriate execution

    // If we did not dispatch to other activity
    // based on commmand line parameters, run
    // the built-in self tests for all of the
    // classes that we know have them.

    Actions::bist();
    Edge::bist();
    Clk8080::bist();

    return 0;
}
