#include "action.hh"
#include "bit.hh"
#include <vector>

class Edge {

    // Edge triggering is only available for
    // single-bit signals.
    Bit val;
    std::vector<Action> rise;
    std::vector<Action> fall;

public:
    Edge(bool init = false);

    // Edge::get() returns the value of the signal.
    // This should be "true" when rising-edge callbacks are running,
    // and should be "false" when falling-edge callbacks are running.
    bool get();

    // Edge::hi() sets the value to true; if the value was false,
    // the actions attached to the rising edge are run.
    void hi();

    // Edge::lo() sets the value to false; if the value was true,
    // the actions attached to the falling edge are run.
    void lo();

    // Edge::rise_cb(a) adds the action a to the list of actions to
    // run when the value transitions from false to true.
    void rise_cb(Action a);

    // Edge::fall_cb(a) adds the action a to the list of actions to
    // run when the value transitions from true to false.
    void fall_cb(Action a);

    // Edge::bist() performs a built-in self-test of the Edge class,
    // throwing an "assertion failure" if something is wrong.
    static void bist();
};
