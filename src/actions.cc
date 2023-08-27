#include "actions.hh"

void Actions::exec()
{
    while (!empty()) {
        front()();
        pop();
    }
}
