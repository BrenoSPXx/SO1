#undef NDEBUG
#include <assert.h>

#include "MemorySystem.h"
#include "SegmentIterator.h"

void MemorySystem::allocate(size_t bytes, size_t id) {
    MemorySegment* segment = memory_algorithm->get_free_segment(memory_manager, bytes);
    segment = memory_manager->allocate(segment, bytes);
    used_memory.insert({id, segment});
}

void MemorySystem::deallocate(size_t id) {
    MemorySegment* segment = used_memory[id];
    used_memory.erase(id);
    memory_manager->deallocate(segment);
}

