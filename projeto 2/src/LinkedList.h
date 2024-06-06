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

    Node* head;
    Node* tail;

    LinkedList();
    ~LinkedList();

    void append(T data);
    void prepend(T data);
    void remove(T data);
    void insertAfter(Node* node, T data);
    T getNextDataFromValue(T value);
    bool isEmpty(); 

    Node* getHead() const { return head; }
    Node* getTail() const { return tail; }
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
T LinkedList<T>::getNextDataFromValue(T value) {
    Node* current = head;
    while (current != nullptr) {
        if (current->data == value && current->next != nullptr) {
            return current->next->data;
        }
        current = current->next;
    }
    throw std::out_of_range("No next node data available for the given value.");
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
}

template<typename T>
void LinkedList<T>::prepend(T data) {
    Node* newNode = new Node(data);
    if (head == nullptr) {
        head = tail = newNode;
    } else {
        newNode->next = head;
        head->prev = newNode;
        head = newNode;
    }
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
    
}

template<typename T>
bool LinkedList<T>::isEmpty(){
    return head == nullptr;
}
