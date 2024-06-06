#pragma once

#include <stdint.h>

#include "MemorySegment.h"
#include "SegmentIterator.h"

class MemoryManager {
protected:
    uint8_t* memory;
    size_t total_size;
    size_t bin_size;

    class StatisticsData {
    private:
        size_t total_allocated;
        size_t total_deallocated;
        int num_allocations;
        int num_deallocations;

    public:
        StatisticsData() :
            total_allocated(0), total_deallocated(0),
            num_allocations(0), num_deallocations(0) {}

        size_t get_total_allocated()   { return total_allocated; }
        size_t get_total_deallocated() { return total_deallocated; }
        int get_num_allocations()      { return num_allocations; }
        int get_num_deallocations()    { return num_deallocations; }

        void allocate(size_t bytes) {
            total_allocated += bytes;
            num_allocations++;
        }

        void deallocate(size_t bytes) {
            total_deallocated += bytes;
            num_deallocations++;
        }
    };

    StatisticsData statistics;

public:
    MemoryManager(size_t total_size_, size_t bin_size_) : total_size(total_size_), bin_size(bin_size_) {
        memory = new uint8_t[total_size];
    }
    virtual ~MemoryManager() {
        delete memory;
    }

    virtual bool auto_delete() = 0;

    void print_statistics();

    virtual void print_specific() = 0;

    [[nodiscard]]
    virtual MemorySegment* allocate(MemorySegment* segment, size_t bytes) = 0;
    virtual void deallocate(MemorySegment* segment) = 0;

    virtual SegmentIterator* get_segment_iterator() = 0;
};
