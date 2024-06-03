#include <iostream>
#include <vector>

#include "Input.h"
#include "MemorySystem.h"
#include "BitmapManager.h"

using std::vector;
using std::cout;
using std::cin;

int main() {
    Input input;
    input.parse();

    BitmapManager bitmap_manager(input.get_memory_size(), input.get_block_size());
    MemoryAlgorithm memory_algorithm;
    MemorySystem system(&bitmap_manager, &memory_algorithm);

    for (Input::Operation& operation : input.get_operations()) {
        char action       = operation.get_action();
        size_t id         = operation.get_id();
        size_t alloc_size = operation.get_allocation_size();

        if (action == 'A') {
            system.allocate(alloc_size, id);
        } else if (action == 'D') {
            system.deallocate(id);
        }
    }
}
