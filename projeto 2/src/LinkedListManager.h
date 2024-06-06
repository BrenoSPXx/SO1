#pragma once

#include <stdlib.h>

#include "MemoryManager.h"
#include "SegmentIterator.h"
#include "LinkedList.h"
#include "MemorySegment.h"
#include <vector>

class LinkedListManager : public MemoryManager {
public:
    // Classe interna MemorySegment que define a estrutura dos segmentos de memória
    class Segment : public MemorySegment{
        public:
            Segment(bool free, size_t bin_id, size_t size);
            void set_free(bool value);
            void set_bin_id(size_t value);
            void set_size(size_t value);
    };
    class Iterator : public SegmentIterator {
      private:
        bool first = true; 
        MemorySegment* segment;
        LinkedListManager* manager;

      public:
        Iterator(MemorySegment* segment_, LinkedListManager* manager_);

        virtual MemorySegment* next() override;
    };

    private:
        // Utiliza a LinkedList para armazenar os segmentos de memória
        LinkedList<Segment*> segments;

    public:
        // Construtor e destrutor
        LinkedListManager(size_t total_size, size_t bin_size);
        ~LinkedListManager();

        // Métodos de alocação e dealocação de memória
        MemorySegment* allocate(size_t bytes);
        void deallocate(MemorySegment* segment);
        virtual SegmentIterator* get_segment_iterator() override;
        std::vector<Segment*> get_segments() const;
};


