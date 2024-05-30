#pragma once

#include "CPU.cpp"
#include "INE5412Context.cpp"

class INE5412 : public CPU {
private:
    INE5412Context context;

public:
    INE5412() {}

    virtual void run() override {
        context.registers[1]++;
        CPU::run();
    }

    virtual CPUContext* new_context() override {
        return new INE5412Context;
    }

    virtual void set_context(CPUContext* other_context) override {
        context.copy(other_context);
    }

    virtual CPUContext const* get_context() override {
        return &context;
    }
};

