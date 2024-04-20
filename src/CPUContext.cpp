#pragma once

class CPUContext {
public:
    CPUContext() {}

    CPUContext(CPUContext const&) = delete;
    CPUContext(CPUContext&&) = delete;
    CPUContext& operator=(CPUContext const&) = delete;
    CPUContext& operator=(CPUContext&&) = delete;

    virtual void set_register(int value) = 0;
    virtual void copy(CPUContext const*) = 0;

    virtual ~CPUContext() {}
};

