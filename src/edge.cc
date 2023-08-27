#include "edge.hh"

Edge::Edge(bool init)
    : val(init)
{
}

bool Edge::get()
{
    return val;
}

void Edge::hi()
{
    if (!val) {
        val = true;
        for (auto& f : rise)
            f();
    }
}

void Edge::lo()
{
    if (val) {
        val = false;
        for (auto& f : fall)
            f();
    }
}

void Edge::set(unsigned u)
{
    return u ? hi() : lo();
}

void Edge::inv(unsigned u)
{
    return u ? lo() : hi();
}

void Edge::rise_cb(Action f)
{
    rise.push_back(f);
}

void Edge::fall_cb(Action f)
{
    fall.push_back(f);
}

