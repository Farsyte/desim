#pragma once

class VerifyElapsedTime {
public:
    VerifyElapsedTime(const char* name, double lb, double ub);

    const char* name;
    double      lb;
    double      ub;

    double lo;
    double hi;

    double from;
    double elapsed;

    void start(double ns);

    void record(double ns);

    void print();
};
