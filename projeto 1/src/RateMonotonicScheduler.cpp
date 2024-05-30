#pragma once

#include <climits>

#include "Scheduler.cpp"

class RateMonotonicScheduler : public Scheduler {
public:
    virtual int next_process(std::vector<PCB> const& process_table) override {
        int min_period = INT_MAX;
        int min_pid = -1;

        for (PCB const& pcb: process_table) {
            if (pcb.period < min_period) {
                min_period = pcb.period;
                min_pid = pcb.pid;
            }
        }
        return min_pid;
    }
};

