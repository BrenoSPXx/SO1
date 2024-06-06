#include <assert.h>
#include "BestFitAlgorithm.h"

MemorySegment* BestFitAlgorithm::get_free_segment(MemoryManager* memory_manager, size_t bytes) {
    MemorySegment* segment;
    SegmentIterator* iterator = memory_manager->get_segment_iterator();

    MemorySegment* best_segment = 0;

    while (true) {
        segment = iterator->next();

        if (!segment) break;

        if (segment->is_free() && segment->get_size() >= bytes) {
            if ((best_segment == NULL) || (segment->get_size() < best_segment->get_size())) {
                if (best_segment != NULL) {
                    delete best_segment;
                }
                best_segment = segment->create_copy();
            }
        }
    }

    return best_segment;
}
