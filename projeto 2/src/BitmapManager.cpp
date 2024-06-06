#include <iostream>

#include "BitmapManager.h"

MemorySegment* BitmapManager::Segment::create_copy() {
    return new Segment(free, bin_id, size);
}

BitmapManager::BitmapManager(size_t total_size, size_t bin_size) : MemoryManager(total_size, bin_size) {
    size_t num_bins = (total_size + bin_size-1) / bin_size;
    size_t num_bin_bytes = (num_bins + 8-1) / 8;
    bitmap = new uint8_t[num_bin_bytes]();
}

BitmapManager::~BitmapManager() {
    delete[] bitmap;
}

void BitmapManager::print_specific() {
    size_t num_bins = (total_size + bin_size-1) / bin_size;

    for (size_t bin_i = 0; bin_i < num_bins; bin_i++) {
        size_t bin_byte = bin_i / 8;
        size_t bin_byte_offset = bin_i % 8;
        std::cout << ((bool)(bitmap[bin_byte] & (1 << bin_byte_offset)));
    }

    std::cout << "\n";
}

MemorySegment* BitmapManager::allocate(MemorySegment* segment, size_t bytes) {
    if (!segment->is_free() || segment->get_size() < bytes) {
        delete segment;
        return 0;
    }

    statistics.allocate(bytes);

    size_t total_bins = (bytes + bin_size-1) / bin_size;
    size_t start_bin = segment->get_bin_id();
    size_t end_bin = start_bin + total_bins;

    for (size_t bin_i = start_bin; bin_i < end_bin; bin_i++) {
        size_t bin_byte = bin_i / 8;
        size_t bin_byte_offset = bin_i % 8;
        bitmap[bin_byte] |= (1 << bin_byte_offset);
    }

    delete segment;
    return new Segment(false, start_bin, bytes);
}

void BitmapManager::deallocate(MemorySegment* segment) {
    statistics.deallocate(segment->get_size());

    size_t total_bins = (segment->get_size() + bin_size-1) / bin_size;
    size_t start_bin = segment->get_bin_id();
    size_t end_bin = start_bin + total_bins;

    for (size_t bin_i = start_bin; bin_i < end_bin; bin_i++) {
        size_t bin_byte = bin_i / 8;
        size_t bin_byte_offset = bin_i % 8;
        bitmap[bin_byte] &= ~(1 << bin_byte_offset);
    }

    delete segment;
}

auto BitmapManager::get_segment(size_t bin_start) -> Segment* {
    size_t bin_i = bin_start;
    size_t num_bins = (total_size + bin_size-1) / bin_size;

    if (bin_i >= num_bins) return 0;

    size_t bin_byte, bin_byte_offset;
    bin_byte = bin_i / 8;
    bin_byte_offset = bin_i % 8;

    bool occupied_segment = (bool)(bitmap[bin_byte] & (1 << bin_byte_offset));

    while (true) {
        bin_i++;

        if (bin_i >= num_bins) break;

        bin_byte = bin_i / 8;
        bin_byte_offset = bin_i % 8;

        if (occupied_segment != (bool)(bitmap[bin_byte] & (1 << bin_byte_offset))) {
            break;
        }
    }

    return new Segment(!occupied_segment, bin_start, (bin_i - bin_start) * bin_size);
}

SegmentIterator* BitmapManager::get_segment_iterator() {
    return new Iterator(this);
}

BitmapManager::Iterator::Iterator(BitmapManager* manager_) : manager(manager_) {}

BitmapManager::Iterator::~Iterator() {
    if (segment) delete segment;
}

MemorySegment* BitmapManager::Iterator::next() {
    if (first) {
        first = false;
        segment = manager->get_segment(0);
        return segment;
    }

    size_t next_bin = segment->get_bin_id() + segment->get_size() / manager->bin_size;
    Segment* new_segment = manager->get_segment(next_bin);

    delete segment;
    segment = new_segment;
    return new_segment;
}

