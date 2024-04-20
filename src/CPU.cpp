#pragma once

#include <unistd.h>

#include "CPUContext.cpp"

class CPU {
private:
    int time;

public:
    CPU() : time(0) {}

    CPU(CPU const&) = delete;
    CPU(CPU&&) = delete;
    CPU& operator=(CPU const&) = delete;
    CPU& operator=(CPU&&) = delete;

    virtual void run() {
        sleep(1);
        time++;
    }

    int get_time() { return time; }

    virtual CPUContext* new_context() = 0;
    virtual void set_context(CPUContext* context) = 0;
    virtual CPUContext const* get_context() = 0;
};

