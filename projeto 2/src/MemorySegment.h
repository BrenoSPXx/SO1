#pragma once

#include <stdlib.h>

class MemorySegment {
protected:
    bool free;
    size_t bin_id;
    size_t size;

public:
    MemorySegment(bool free_, size_t bin_id_, size_t size_) : free(free_), bin_id(bin_id_), size(size_) {}
    virtual ~MemorySegment() {}

    virtual MemorySegment* create_copy() = 0;

    bool   is_free()    { return free; }
    size_t get_bin_id() { return bin_id; }
    size_t get_size()   { return size; }
};
