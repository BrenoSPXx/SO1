#pragma once

#include <stdlib.h>
#include <map>

#include "MemoryManager.h"
#include "SegmentIterator.h"

class BitmapManager : public MemoryManager {
private:
    uint8_t* bitmap = 0;

    class Segment : public MemorySegment {
      public:
        Segment(bool free_, size_t bin_id_, size_t size_) : MemorySegment(free_, bin_id_, size_) {}

        virtual MemorySegment* create_copy() override;
    };

    class Iterator : public SegmentIterator {
      private:
        bool first = true;
        MemorySegment* segment = 0;
        BitmapManager* manager;

      public:
        Iterator(BitmapManager* manager_);
        virtual ~Iterator() override;

        virtual MemorySegment* next() override;
    };

    Segment* get_segment(size_t bin_start);

public:
    BitmapManager(size_t total_size, size_t bin_size);
    virtual ~BitmapManager() override;

    BitmapManager(BitmapManager const&) = delete;
    BitmapManager(BitmapManager &&)     = delete;
    BitmapManager& operator=(BitmapManager const&) = delete;
    BitmapManager& operator=(BitmapManager&&)      = delete;

    virtual bool auto_delete() override { return false; }
    virtual void print_specific() override;

    [[nodiscard]]
    virtual MemorySegment* allocate(MemorySegment* segment, size_t bytes) override;

    virtual void deallocate(MemorySegment* segment) override;

    virtual SegmentIterator* get_segment_iterator() override;
};
