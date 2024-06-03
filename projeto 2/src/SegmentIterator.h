#pragma once

#include <iterator>

class SegmentIterator {
protected:
    MemorySegment* segment;

    SegmentIterator(MemorySegment* segment_);
    virtual void copy(SegmentIterator&) = 0;

public:
    virtual ~SegmentIterator();

    using difference_type = ptrdiff_t;
    using value_type = MemorySegment*;
    using reference = MemorySegment*&;
    using pointer = MemorySegment**;
    using iterator_category = std::input_iterator_tag;

    SegmentIterator(SegmentIterator const&);
    SegmentIterator& operator=(SegmentIterator const&);

    SegmentIterator(SegmentIterator&&) = delete;
    SegmentIterator& operator=(SegmentIterator&&) = delete;

    virtual void swap(SegmentIterator&);
    friend void swap(SegmentIterator& lhs, SegmentIterator& rhs);

    virtual MemorySegment*& operator*() const;
    virtual MemorySegment** operator->();
    virtual SegmentIterator& operator++() = 0;
    virtual SegmentIterator& operator++(int) = 0;

    virtual bool operator==(SegmentIterator& other);
    virtual bool operator!=(SegmentIterator& other);
};
