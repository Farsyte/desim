#include <functional>
#include <queue>

#include "action.hh"

class Actions : public std::queue<Action> {
public:
    inline void exec()
    {
        front()();
        pop();
    }

    static void bist();
};
