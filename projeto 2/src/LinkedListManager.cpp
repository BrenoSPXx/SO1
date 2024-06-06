#include <cmath>
#include <iostream>

#include "LinkedListManager.h"

LinkedListManager::Segment::Segment(bool free, size_t bin_id, size_t size) : MemorySegment(free, bin_id, size) {}
LinkedListManager::Iterator::Iterator(MemorySegment* segment_, LinkedListManager* manager_) : segment(segment_), manager(manager_) {}

// Construtor do LinkedListManager inicializa a lista com um segmento grande que representa toda a memória disponível
LinkedListManager::LinkedListManager(size_t total_size, size_t bin_size) : MemoryManager(total_size, bin_size) {
    // Inicializa a lista com um único segmento grande livre
    size_t zero = 0;
    segments.append(new Segment(true, zero, static_cast<size_t>(std::ceil(total_size / bin_size))));
}

// Destrutor do LinkedListManager limpa todos os segmentos
LinkedListManager::~LinkedListManager() {
    typename LinkedList<Segment*>::Node* current = segments.getHead();
    while (current != nullptr) {
        MemorySegment* segment = current->data;
        delete segment;  
        current = current->next;
    }
}

// Aloca um bloco de memória de tamanho especificado em bytes
MemorySegment* LinkedListManager::allocate(MemorySegment* segment, size_t bytes) {
    size_t required_bins = (bytes + bin_size - 1) / bin_size;  // Calcula o número necessário de bins, arredondando para cima

    Segment* seg = (Segment*)segment; 
    for (auto node = segments.getHead(); node != nullptr; node = node->next) {
        if (seg == node->data) {
            if (seg->is_free() && seg->get_size() >= required_bins) {
                if (seg->get_size() > required_bins) {
                    // Se o segmento é maior que o necessário, divide o segmento
                    segments.insertAfter(node, new Segment(true, seg->get_bin_id() + required_bins, seg->get_size() - required_bins));
                    seg->set_size(required_bins);
                }
                seg->set_free(false);
                return seg;
            }
        }
    }
    return nullptr;  // Retorna nulo se nenhum segmento livre adequado for encontrado
}

// Libera um bloco de memória especificado
void LinkedListManager::deallocate(MemorySegment* segment) {

    auto node = segments.getHead();
    while (node) {
        if (node->data == segment) {
            // Coalescência com o segmento anterior, se livre
            if (node->prev && node->prev->data->is_free()) {
                node->prev->data->set_size(node->data->get_size() + node->prev->data->get_size());
                segments.remove((Segment*)(segment));
                node = node->prev;  // Atualiza o node para o segmento expandido
            }

            // Coalescência com o segmento seguinte, se livre
            if (node->next && node->next->data->is_free()) {
                node->data->set_size(node->next->data->get_size() + node->data->get_size());
                segments.remove(node->next->data);
            }
            break;
        }
        node = node->next;
    }
}


std::vector<LinkedListManager::Segment*> LinkedListManager::get_segments() const {
    std::vector<Segment*> all_segments;
    for (auto node = segments.getHead(); node != nullptr; node = node->next) {
        all_segments.push_back(node->data);
    }
    return all_segments;
}

SegmentIterator* LinkedListManager::get_segment_iterator() {
    return new Iterator(segments.getHead()->data, this);
}

MemorySegment* LinkedListManager::Iterator::next() {
    if (first) {
        first = false;
        return segment;
    }
    std::vector<Segment*> segments = manager->get_segments();
    for (size_t i = 0; i < segments.size() - 1; i++) {
        if (segment == segments[i]) {
            segment = segments[i + 1];
            return segment;
        }
    }
    return 0;
}

void LinkedListManager::Segment::set_free(bool value) {
    free = value;
}

void LinkedListManager::Segment::set_bin_id(size_t value) {
    bin_id = value;
}

void LinkedListManager::Segment::set_size(size_t value) {
    size = value;
}

void LinkedListManager::print_specific() {
    std::cout << "Linked List\n";
}
