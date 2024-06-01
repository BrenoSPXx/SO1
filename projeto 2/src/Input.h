#pragma once

#include <vector>

class Input {
private:
    int management_mode;
    size_t memory_size;
    size_t block_size;
    int algorithm;

    class Operation {
    private:
        char ch;
        int id;
        int alloc_size;
    public:
        Operation(char ch_, int id_, int alloc_size_) : ch(ch_), id(id_), alloc_size(alloc_size_) {}
        Operation(char ch_, int id_) : ch(ch_), id(id_), alloc_size(0) {}
    };

    std::vector<Operation> operations;

public:
    void parse();
    size_t get_memory_size()                { return memory_size; } 
    size_t get_block_size()                 { return block_size; }
    std::vector<Operation> get_operations() { return operations; }
};

