#pragma once

#include "MemoryAlgorithm.h"
#include "MemorySegment.h"

class FirstFitAlgorithm : public MemoryAlgorithm {
public:
    virtual MemorySegment* get_free_segment(MemoryManager* memory_manager, size_t bytes) override;
};