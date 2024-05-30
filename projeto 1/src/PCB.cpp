#pragma once

#include "ProcessState.cpp"

struct PCB {
    int pid;

    int creation_time;
    int duration;
    int period;
    int deadline;
    int static_priority;

    int executed_cycles;

    ProcessState state;
    CPUContext* context;
};

