#pragma once

#include <vector>

class Input {
private:
    int management_mode;
    int memory_size;
    int block_size;
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
};

