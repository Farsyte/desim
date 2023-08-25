#include <chrono>

class Timer {
public:
    Timer();
    void tick();
    void tock();
    unsigned long microseconds() const;

protected:
    typedef std::chrono::steady_clock::time_point TP;
    TP _start;
    TP _end;
};
