#include "timer.hh"

using std::chrono::steady_clock;

Timer::Timer()
{
    _start = steady_clock::now();
    _end = {};
}
void Timer::tick()
{
    _end = TP {};
    _start = steady_clock::now();
}
void Timer::tock() { _end = steady_clock::now(); }

unsigned long Timer::microseconds() const
{
    return std::chrono::duration_cast<std::chrono::microseconds>(_end - _start).count();
}
