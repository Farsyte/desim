#include "traced.hh"

//#include <iostream>

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

    //    fprintf(stderr, "initial value of %s%s is '%c'\n",
    //        active_low ? "/" : "",
    //        name, valc);
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
    if (valc == vals[1])
        return;
    update_trace();
    valc = vals[1];
}

void Traced::fall()
{
    if (valc == vals[0])
        return;
    update_trace();
    valc = vals[0];
}

void Traced::print(int tmin, int tlen)
{
    auto s = trace.substr(tmin - u0, tlen);

    bool boring = true;
    for (auto c : s) {
        if (c != ',')
            boring = false;
    }
    if (boring)
        return;

    printf("%s:\t%s\n", name, s.c_str());

    //    std::cout << name << ":\t";
    //    std::cout << s;
    //    std::cout << std::endl;
}
