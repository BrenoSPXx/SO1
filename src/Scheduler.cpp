#pragma once

#include "PCB.cpp"

class Scheduler {
public:
    virtual int next_process(std::vector<PCB> const& process_table) = 0;
};

