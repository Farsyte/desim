#include "sys8080.hh"

#include <cstdio>

#include "timer.hh"
#include "traced.hh"

#include "stub.hh"

//#include "verify_elapsed_time.hh"
//#include <iostream>

// to print a value in binary:
// #include <iostream>
// #include <bitset>
//    std::cout << "cycle: BIT_PHI1 = "
//              << std::bitset<9>(BIT_PHI1) << "\n";

//#include "tau.hh"
//#include "traced.hh"
//#include "verify_elapsed_time.hh"

// static VerifyElapsedTime p1("name_of_timing_constraint", min_ns, max_ns); // which edge to which edge

unsigned      call_count = 100;
static double check_timing(Action a)
{
    Timer t;

    t.tick();
    for (unsigned rep = 0; rep < call_count; ++rep)
        a();
    t.tock();

    unsigned long us_total = t.microseconds();
    if (us_total < 100) {
        call_count *= 10;
        return check_timing(a);
    }
    if (us_total < 100000) {
        call_count = (call_count * 200000) / us_total;
        return check_timing(a);
    }

    double ns_avg = us_total * 1000.0 / call_count;

    //    printf("total time: %lu ms\n", us_total/1000);
    //    printf("call count: %u\n", call_count);
    //    printf("avg time: %.1f ns\n", ns_avg);

    return ns_avg;
}

void Sys8080::bist()
{
    Sys8080& sys = *(Sys8080::create("sys1"));

    Edge& OSC(sys.OSC);

    Action tick = [&] {
        const double  MHz  = 18.00;
        static double pend = 0.00;

        OSC.hi();
        OSC.lo();

        pend += 1000.0 / MHz;
        TAU += (int)pend;
        pend -= (int)pend;
        UNIT++;
    };

    std::function<void(tau_t)> tick_to = [&](tau_t end_tau) {
        while (TAU < end_tau)
            tick();
    };

    std::function<void(double)> tick_us = [&](double us) {
        double dt = us * 1000.0;
        tick_to(TAU + (int)dt);
    };

    sys.linked();

    // do some benchmarking.

    UNIT = 0;
    TAU  = 0;

    sys.RESIN.lo();
    tick_us(1.5);
    sys.RESIN.hi();
    tick_us(2.0);
    sys.RDYIN.hi();

    Action a = [=] { tick_us(1000.0); };

    double ms_per_sim_ms_1 = check_timing(a) / 1000000.0;
    double ms_per_sim_ms_2 = check_timing(a) / 1000000.0;
    double ms_per_sim_ms_3 = check_timing(a) / 1000000.0;
    fprintf(stderr, "Sys8080: wall ms per sim ms is %.2f, %.2f, %.2f\n",
        ms_per_sim_ms_1, ms_per_sim_ms_2, ms_per_sim_ms_3);

    // get back into reset.
    sys.RESIN.lo();
    sys.RDYIN.lo();
    tick_us(2.0);

    // sys.OSC
    // sys.RESIN
    // sys.RDYIN
    // sys.DMARQ
    // sys.INTRQ

    std::vector<Traced*> traces;

    struct PageBreak {
        PageBreak(tau_t t, const char* s)
            : t(t)
            , s(s)
        {
        }
        tau_t       t;
        const char* s;
    };

    std::vector<PageBreak> page_breaks;
    const char*            page_title      = "";
    tau_t                  phi1rise_unit   = 0;
    const char*            phi1rise_title1 = "";
    const char*            phi1rise_title2 = "";

    sys.clk.PHI1.rise_cb([&] {
        phi1rise_unit   = UNIT;
        phi1rise_title2 = phi1rise_title1;
        phi1rise_title1 = page_title;
    });

    Action that_phi1_rise_is_pagebreak = [&] {
        page_breaks.push_back(PageBreak(phi1rise_unit, phi1rise_title2));
    };

    sys.cpu.SYNC.rise_cb(that_phi1_rise_is_pagebreak);

    // need UNIT=0 when we start tracing.
    UNIT = 0;
    TAU  = 0;

    traces.push_back(new Traced("PHI1", sys.clk.PHI1));
    traces.push_back(new Traced("PHI2", sys.clk.PHI2));
    traces.push_back(new Traced("PHI1A", sys.clk.PHI1A));

    traces.push_back(new Traced("RESIN", sys.RESIN, true));
    traces.push_back(new Traced("RESET", sys.clk.RESET));

    traces.push_back(new Traced("SYNC", sys.cpu.SYNC));
    traces.push_back(new Traced("STSTB", sys.clk.STSTB, true));

    traces.push_back(new Traced("RDYIN", sys.RDYIN, true));
    traces.push_back(new Traced("READY", sys.clk.READY, true));

    traces.push_back(new Traced("DMARQ", sys.DMARQ));
    traces.push_back(new Traced("HOLD", sys.clk.HOLD));

    traces.push_back(new Traced("INTRQ", sys.INTRQ));
    traces.push_back(new Traced("INT", sys.clk.INT));

    traces.push_back(new Traced("DBIN", sys.cpu.DBIN));
    traces.push_back(new Traced("WR", sys.cpu.WR, true));
    traces.push_back(new Traced("HLDA", sys.cpu.HLDA));

    traces.push_back(new Traced("MEMR", sys.clk.MEMR, true));
    traces.push_back(new Traced("MEMW", sys.clk.MEMW, true));
    traces.push_back(new Traced("IOR", sys.clk.IOR, true));
    traces.push_back(new Traced("IOW", sys.clk.IOW, true));
    traces.push_back(new Traced("INTA", sys.clk.INTA, true));

    sys.RESIN.lo();
    tick_us(1.5);
    sys.RESIN.hi();
    tick_us(2.0);
    sys.RDYIN.hi();

    tick_us(6.5);

    tau_t u_max = UNIT;

    for (auto* it : traces)
        it->update_trace();

    // Tune "maxm" to limit the width of data plots.
    int maxm = 72;

    page_breaks.push_back(PageBreak(u_max, page_title));

    double us_per_unit = (TAU * 0.001) / UNIT;

    printf("\n");
    tau_t u = 0;
    for (auto m : page_breaks) {
        while (u < m.t) {
            tau_t hi = u + maxm;
            if (hi > m.t)
                hi = m.t;
            printf("\n");
            printf("From %.3f to %.3f μs: %s\n", us_per_unit * u, us_per_unit * hi, m.s);
            for (auto* it : traces)
                it->print(u, hi - u);
            u = hi;
        }
    }
    printf("\n");
    printf("Sys8080::bist complete.\n");
}