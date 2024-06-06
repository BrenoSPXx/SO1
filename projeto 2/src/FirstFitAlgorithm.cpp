#include <assert.h>
#include "FirstFitAlgorithm.h"

MemorySegment* FirstFitAlgorithm::get_free_segment(MemoryManager* memory_manager, size_t bytes) {
    MemorySegment* segment = 0;
    SegmentIterator* iterator = memory_manager->get_segment_iterator();

    while (true) {
        segment = iterator->next();
        assert(segment);

        if (segment->is_free() && segment->get_size() >= bytes) {
            MemorySegment* copy_segment = segment->create_copy();
            delete iterator;
            return copy_segment;
        }
    }
}
