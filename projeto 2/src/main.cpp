#include <iostream>
#include <vector>

#include "Input.h"
#include "BitmapManager.h"
#include "LinkedListManager.h"
#include "FirstFitAlgorithm.h"
#include "BestFitAlgorithm.h"
#include "MemorySystem.h"

using std::vector;
using std::cout;
using std::cin;

int main() {
    Input input;
    input.parse();

    MemoryManager* memory_manager = 0;
    if (input.get_management_mode() == 1) {
        memory_manager = new BitmapManager(input.get_memory_size(), input.get_block_size());
    } else if (input.get_management_mode() == 2) {
        memory_manager = new LinkedListManager(input.get_memory_size(), input.get_block_size());
    }

    MemoryAlgorithm* memory_algorithm = 0;
    if (input.get_algorithm() == 1) {
        memory_algorithm = new FirstFitAlgorithm();
    } else if (input.get_algorithm() == 2) {
        memory_algorithm = new BestFitAlgorithm();
    }

    MemorySystem system(memory_manager, memory_algorithm);

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

    memory_manager->print_statistics();
    memory_manager->print_specific();

    delete memory_manager;
    delete memory_algorithm;
}
