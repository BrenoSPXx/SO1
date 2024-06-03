#include "SegmentIterator.h"

SegmentIterator::SegmentIterator(MemorySegment* segment_) : segment(segment_) {}

~SegmentIterator() {};

SegmentIterator::SegmentIterator(SegmentIterator const& other) : segment(other.segment) {
    this->copy(other);
}

SegmentIterator::SegmentIterator& operator=(SegmentIterator const& other) {
    segment = other.segment;
    this->copy(other);
}

void SegmentIterator::swap(SegmentIterator& other) {
    MemorySegment* temp = other.segment;
    other.segment = segment;
    segment = temp;
}

MemorySegment*& SegmentIterator::operator*() const {
    return segment;
}

bool SegmentIterator::operator==(SegmentIterator& other) {
    return segment == other.segment;
}

bool SegmentIterator::operator!=(SegmentIterator& other) {
    return segment != other.segment;
}

MemorySegment** SegmentIterator::operator->() {
    return &memory;
}

void swap(SegmentIterator& lhs, SegmentIterator& rhs) {
    lhs.swap(rhs);
}
