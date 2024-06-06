#include <iostream>

#include "MemoryManager.h"

void MemoryManager::print_statistics() {
    std::cout << statistics.get_total_allocated() - statistics.get_total_deallocated() << "\n";
    std::cout << statistics.get_total_allocated() << "\n";
    std::cout << statistics.get_total_deallocated() << "\n";
    std::cout << statistics.get_num_allocations() << "\n";
    std::cout << statistics.get_num_deallocations() << "\n";
}
