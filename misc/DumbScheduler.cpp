#pragma once

class DumbScheduler : public Scheduler {
public:
    virtual int next_process(std::vector<PCB> const& process_table) override {
        int min_pid = -1;
        for (PCB const& pcb : process_table) {
            if (min_pid == -1 || pcb.pid < min_pid) min_pid = pcb.pid;
        }
        return min_pid;
    }
};

