#include "BitmapManager.h"

BitmapManager::BitmapManager(size_t total_size, size_t bin_size) : MemoryManager(total_size, bin_size) {
    size_t num_bins = (total_size + bin_size-1) / bin_size;
    size_t num_bin_bytes = (num_bins + 8-1) / 8;
    bitmap = new uint8_t[num_bin_bytes]();
}

BitmapManager::~BitmapManager() {
    delete[] bitmap;
}

void BitmapManager::allocate(MemorySegment** segment, size_t bytes) {
    if (!(*segment)->is_free() || (*segment)->get_size() < bytes) {
        delete *segment;
        *segment = 0;
        return;
    }

    size_t total_bins = (bytes + bin_size-1) / bin_size;
    size_t start_bin = (*segment)->get_bin_id();
    size_t end_bin = start_bin + total_bins;

    for (size_t bin_i = start_bin; bin_i < end_bin; bin_i++) {
        size_t bin_byte = bin_i / 8;
        size_t bin_byte_offset = bin_i % 8;
        bitmap[bin_byte] |= (1 << bin_byte_offset);
    }

    delete *segment;
    *segment = new MemorySegment(false, start_bin, bytes);
}

void BitmapManager::deallocate(MemorySegment* segment) {
    size_t total_bins = (segment->get_size() + bin_size-1) / bin_size;
    size_t start_bin = segment->get_bin_id();
    size_t end_bin = start_bin + total_bins;

    for (size_t bin_i = start_bin; bin_i < end_bin; bin_i++) {
        size_t bin_byte = bin_i / 8;
        size_t bin_byte_offset = bin_i % 8;
        bitmap[bin_byte] &= ~(1 << bin_byte_offset);
    }
}

// TODO: delete
size_t BitmapManager::bitmap_size() {
    size_t num_bins = (total_size + bin_size-1) / bin_size;
    size_t num_bin_bytes = (num_bins + 8-1) / 8;
    return num_bin_bytes;
}
