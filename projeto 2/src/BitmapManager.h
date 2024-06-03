#pragma once

#include <stdlib.h>
#include <map>

#include "MemoryManager.h"
#include "SegmentIterator.h"

class BitmapManager : public MemoryManager {
public:
    // TODO: move to private
    class Segment : public MemorySegment {
      public:
        Segment(bool free_, size_t bin_id_, size_t size_) : MemorySegment(free_, bin_id_, size_) {}
    };
    class Iterator : public SegmentIterator {
      private:
        bool first = true;
        MemorySegment* segment;
        BitmapManager* manager;

      public:
        Iterator(Segment* segment, BitmapManager* manager_);

        virtual MemorySegment* next() override;
    };


private:
    uint8_t* bitmap = 0;

    // TODO: delete
    size_t bitmap_size();

    Segment* get_segment(size_t bin_start);

public:
    BitmapManager(size_t total_size, size_t bin_size);
    ~BitmapManager();

    BitmapManager(BitmapManager const&) = delete;
    BitmapManager(BitmapManager &&)     = delete;
    BitmapManager& operator=(BitmapManager const&) = delete;
    BitmapManager& operator=(BitmapManager&&)      = delete;

    [[nodiscard]]
    virtual MemorySegment* allocate(MemorySegment* segment, size_t bytes) override;

    virtual void deallocate(MemorySegment* segment) override;

    virtual SegmentIterator* get_segment_iterator() override;
};
