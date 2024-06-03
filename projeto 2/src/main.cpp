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
    system.allocate(17, 1);
}
