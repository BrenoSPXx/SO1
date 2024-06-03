#pragma once

#include <stdint.h>

#include "MemorySegment.h"

class MemoryManager {
protected:
    uint8_t* memory;
    size_t total_size;
    size_t bin_size;
public:
    MemoryManager(size_t total_size_, size_t bin_size_) : total_size(total_size_), bin_size(bin_size_) {
        memory = new uint8_t[total_size];
    }

    [[nodiscard]]
    virtual MemorySegment* allocate(MemorySegment* segment, size_t bytes) = 0;
    virtual void deallocate(MemorySegment* segment) = 0;
};
