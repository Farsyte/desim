#pragma once

#include <queue>

#include "action.hh"

class Actions : public std::queue<Action> {
public:
    // exec: pop and run every Action in the queue.
    void exec();

    static void bist();
};
