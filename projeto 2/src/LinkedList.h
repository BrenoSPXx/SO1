#pragma once

#include <iostream>
#include <stdlib.h>

template<typename T>
class LinkedList {
public:
    struct Node {
        T data;
        Node* next;
        Node* prev;

        Node(T val) : data(val), next(nullptr), prev(nullptr) {}
    };
    private:
        Node* head;
        Node* tail;
        size_t size_ = 0;

    public:
        LinkedList();
        ~LinkedList();

        void append(T data);
        void remove(T data);
        void insertAfter(Node* node, T data);
        bool isEmpty(); 
        size_t size();
        Node* getHead() const { return head; }
};

template<typename T>
LinkedList<T>::LinkedList() : head(nullptr), tail(nullptr) {}

template<typename T>
LinkedList<T>::~LinkedList() {
    Node* current = head;
    while (current != nullptr) {
        Node* next = current->next;
        delete current;
        current = next;
    }
}


template<typename T>
void LinkedList<T>::append(T data) {
    Node* newNode = new Node(data);
    if (tail == nullptr) {
        head = tail = newNode;
    } else {
        tail->next = newNode;
        newNode->prev = tail;
        tail = newNode;
    }
    size_ += 1;
}

template<typename T>
void LinkedList<T>::remove(T data) {
    Node* current = head;
    while (current != nullptr && !(current->data == data)) {
        current = current->next;
    }
    if (current == nullptr) {
        return;
    }

    if (current->prev != nullptr) {
        current->prev->next = current->next;
    } else {
        head = current->next;
    }
    
    if (current->next != nullptr) {
        current->next->prev = current->prev;
    } else {
        tail = current->prev;
    }

    delete current;
    size_ -= 1;
}

template<typename T>
void LinkedList<T>::insertAfter(Node* node, T data) {
    if (node == nullptr) return;

    Node* newNode = new Node(data);
    newNode->prev = node;
    newNode->next = node->next;
    if (node->next) {
        node->next->prev = newNode;
    }
    node->next = newNode;
    size_ += 1;
}

template<typename T>
bool LinkedList<T>::isEmpty(){
    return head == nullptr;
}

template<typename T>
size_t LinkedList<T>::size() {
    return size_;
}
