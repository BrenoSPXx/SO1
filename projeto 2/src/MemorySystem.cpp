#undef NDEBUG
#include <assert.h>

#include "MemorySystem.h"
#include "SegmentIterator.h"

void MemorySystem::allocate(size_t bytes, size_t id) {
    MemorySegment* segment;
    {
        // TODO: substitute with MemoryAlgorithm call
        SegmentIterator* iterator = memory_manager->get_segment_iterator();
        while (true) {
            segment = iterator->next();
            assert(segment);

            if (segment->is_free() && segment->get_size() >= bytes) break;
        }
    }

    segment = memory_manager->allocate(segment, bytes);
    used_memory.insert({id, segment});
}

void MemorySystem::deallocate(size_t id) {
    MemorySegment* segment = used_memory[id];
    used_memory.erase(id);
    memory_manager->deallocate(segment);
}
