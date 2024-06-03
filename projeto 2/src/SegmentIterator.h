#pragma once

#include <iterator>

class SegmentIterator {
public:
    virtual MemorySegment* next() = 0;
};
