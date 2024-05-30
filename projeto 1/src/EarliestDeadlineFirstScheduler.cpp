#pragma once

#include "Scheduler.cpp"

class EarliestDeadlineFirstScheduler : public Scheduler {
public:
    virtual int next_process(std::vector<PCB> const& process_table) override {
        int min_abs_deadline = INT_MAX;
        int min_pid = -1;

        for (PCB const& pcb : process_table) {
            if (pcb.deadline + pcb.creation_time < min_abs_deadline) {
                min_abs_deadline = pcb.deadline + pcb.creation_time;
                min_pid = pcb.pid;
            }
        }
        return min_pid;
    }
};

