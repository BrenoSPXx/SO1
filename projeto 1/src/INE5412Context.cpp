#pragma once

#include <cassert>
#include <cstring>

class INE5412Context : public CPUContext {
    friend class INE5412;

private:
    uint64_t registers[6]{};
    uint64_t sp{};
    uint64_t pc{};
    uint64_t st{};

public:
    INE5412Context() : registers{}, sp{}, pc{}, st{} {}

    virtual void copy(CPUContext const* other_) override {
        assert(dynamic_cast<INE5412Context const*>(other_));

        INE5412Context const& other = (INE5412Context const&)*other_;

        memcpy(registers, other.registers, sizeof(registers));
        sp = other.sp;
        pc = other.pc;
        st = other.st;
    }

    virtual void set_register(int value) override {
        registers[0] = value;
    }

    virtual ~INE5412Context() override {}
};

