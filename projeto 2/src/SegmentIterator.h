#pragma once

#include <iterator>

class SegmentIterator {
public:
    virtual ~SegmentIterator() {}
    virtual MemorySegment* next() = 0;
};
