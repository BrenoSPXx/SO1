#pragma once

#include <map>
#include <stdlib.h>

#include "MemoryManager.h"

// TODO: delete
class MemoryAlgorithm {};

class MemorySystem {
private:
    MemoryManager* memory_manager;
    MemoryAlgorithm* memory_algorithm;
    std::map<size_t, MemorySegment*> used_memory;

public:
    MemorySystem(MemoryManager* memory_manager_, MemoryAlgorithm* memory_algorithm_)
        : memory_manager(memory_manager_), memory_algorithm(memory_algorithm_) {}

    ~MemorySystem() {
        for (std::pair<size_t, MemorySegment*> curr_pair : used_memory) {
            delete curr_pair.second;
        }
    }

    void allocate(size_t bytes, size_t id);
    void deallocate(size_t id);
};
