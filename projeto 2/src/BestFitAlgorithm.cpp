#include <assert.h>
#include "BestFitAlgorithm.h"

MemorySegment* BestFitAlgorithm::get_free_segment(MemoryManager* memory_manager, size_t bytes) {
    MemorySegment* segment;
    SegmentIterator* iterator = memory_manager->get_segment_iterator();
    MemorySegment* best_segment;

    while (true) {
        segment = iterator->next();
        assert(segment);
        best_segment = NULL;

        if (segment->is_free() && segment->get_size() >= bytes) {
            if ((best_segment == NULL) || (segment->get_size() < best_segment->get_size())) {
                best_segment = segment;
            }
        }
    }
    return best_segment;
}