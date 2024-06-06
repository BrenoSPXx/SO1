#pragma once

#include "MemorySegment.h"
#include "MemoryManager.h"
#include "SegmentIterator.h"

class MemoryAlgorithm {
public:
    virtual MemorySegment* get_free_segment(MemoryManager* memory_manager, size_t bytes) = 0;
    virtual ~MemoryAlgorithm() {};
};
