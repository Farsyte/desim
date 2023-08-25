#include "traced.hh"

#include <iostream>

Traced::Traced(const char* name, Edge& e, bool active_low)
    : name(name)
    , valc(' ')
    , u0(UNIT)
    , u(UNIT)
{
    if (active_low)
        vals = "#,";
    else
        vals = ",#";

    Edge_RISE(&e, rise);
    Edge_FALL(&e, fall);

    valc = vals[e.get()];
}

void Traced::update_trace()
{
    while (u < UNIT) {
        trace.push_back(valc);
        u++;
    }
}

void Traced::rise()
{
    update_trace();
    valc = vals[1];
}

void Traced::fall()
{
    update_trace();
    valc = vals[0];
}

void Traced::print(int tmin, int tlen)
{
    auto s = trace.substr(tmin - u0, tlen);

    std::cout << name << ":\t";
    std::cout << s;
    std::cout << std::endl;
}
