#include <assert.h>
#include "FirstFitAlgorithm.h"

MemorySegment* FirstFitAlgorithm::get_free_segment(MemoryManager* memory_manager, size_t bytes) {
    MemorySegment* segment;
    SegmentIterator* iterator = memory_manager->get_segment_iterator();

    while (true) {
        segment = iterator->next();
        assert(segment);

        if (segment->is_free() && segment->get_size() >= bytes) {
            return segment->create_copy();
        }
    }
}
