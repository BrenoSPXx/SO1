#include "MemorySystem.h"
// TODO: delete
#include "BitmapManager.h"

void MemorySystem::allocate(size_t bytes, size_t id) {
    MemorySegment* segment = new BitmapManager::Segment(true, 2, 19);
    segment = memory_manager->allocate(segment, bytes);
    used_memory.insert({id, segment});
}

void MemorySystem::deallocate(size_t id) {
    MemorySegment* segment = used_memory[id];
    used_memory.erase(id);
    memory_manager->deallocate(segment);
}
