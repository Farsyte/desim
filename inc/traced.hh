#include <string>

#include "edge.hh"
#include "tau.hh"

class Traced {
public:
    Traced(const char* name, Edge& sig, bool active_low = false);

    std::string name;
    std::string trace;
    char valc;
    const char* vals;

    tau_t u0;
    tau_t u;

    void update_trace();
    void rise();
    void fall();

    std::string fancy(std::string& s);

    void print(int tmin, int tlen);
};
