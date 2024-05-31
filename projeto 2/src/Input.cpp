#include <iostream>

#include "Input.h"

void Input::parse() {
    std::cin >> management_mode >> memory_size >> block_size >> algorithm;

    while (true) {
        char c;
        std::cin >> c;

        if (!std::cin) break;

        if (c == 'A') {
            int alloc_size, id;
            std::cin >> alloc_size >> id;
            operations.emplace_back('A', id, alloc_size);
        } else if (c == 'D') {
            int id;
            std::cin >> id;
            operations.emplace_back('D', id);
        }
    }
}
