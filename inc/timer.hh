#include <chrono>

// The Timer class provides a simple way for test code
// to measure elapsed time between two designated places
// in the test code.

class Timer {
public:
    Timer();

    // The Timer::tick() function establishes the starting time
    // for the time interval to measure.
    void tick();

    // The Timer::tick() function establishes the ending time
    // for the time interval to measure.
    void tock();

    // The Timer::microseconds() function determines from the
    // data recorded by tick() and tock() how many microseconds
    // of wall-clock time elapsed between them.
    unsigned long microseconds() const;

protected:
    typedef std::chrono::steady_clock::time_point TP;

    TP _start;
    TP _end;
};
