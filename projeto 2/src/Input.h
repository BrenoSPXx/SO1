#pragma once

#include <vector>

class Input {
public:
    class Operation {
    private:
        char action;
        size_t id;
        size_t alloc_size;
    public:
        Operation(char action_, size_t id_, size_t alloc_size_) : action(action_), id(id_), alloc_size(alloc_size_) {}
        Operation(char action_, size_t id_) : action(action_), id(id_), alloc_size(0) {}

        char get_action()            { return action; }
        size_t get_id()              { return id; }
        size_t get_allocation_size() { return alloc_size; }
    };

private:
    int management_mode;
    size_t memory_size;
    size_t block_size;
    int algorithm;

    std::vector<Operation> operations;

public:
    void parse();
    size_t get_memory_size()                 { return memory_size; } 
    size_t get_block_size()                  { return block_size; }
    std::vector<Operation>& get_operations() { return operations; }
};

