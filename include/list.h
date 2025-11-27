#pragma once
#include <memory_resource>
#include <cstddef>

template <typename T>
struct Node {
    T value;
    Node* next = nullptr;
    Node* prev = nullptr;
    Node(const T& v) : value(v) {}
};

template <typename T>
class Iterator {
public:
    using value_type = T;
    using pointer = T*;
    using reference = T&;

private:
    Node<T>* current;

public:
    Iterator(Node<T>* node) : current(node) {}
    
    reference operator*() const { return current->value; }
    pointer operator->() const { return &current->value; }
    
    Iterator& operator++() {
        if (current) current = current->next;
        return *this;
    }
    
    Iterator operator++(int) {
        Iterator temp = *this;
        ++(*this);
        return temp;
    }
    
    bool operator==(const Iterator& other) const { return current == other.current; }
    bool operator!=(const Iterator& other) const { return current != other.current; }
};

template <typename T>
class DoubleLinkedList {
private:
    std::pmr::polymorphic_allocator<Node<T>> allocator;
    Node<T>* head = nullptr;
    Node<T>* tail = nullptr;

public:
    DoubleLinkedList(std::pmr::memory_resource* mr = std::pmr::get_default_resource())
        : allocator(mr) {}
    
    ~DoubleLinkedList() {
        clear();
    }

    void push_back(const T& value) {
        Node<T>* newNode = allocator.allocate(1);
        allocator.construct(newNode, value);
        if (!tail) {
            head = tail = newNode;
        } else {
            tail->next = newNode;
            newNode->prev = tail;
            tail = newNode;
        }
    }

    void pop_back() {
        if (!tail) return;
        Node<T>* temp = tail;
        tail = tail->prev;
        
        if (tail) 
            tail->next = nullptr;
        else 
            head = nullptr;
        allocator.destroy(temp);
        allocator.deallocate(temp, 1);
    }

    void clear() {
        while (head) {
            pop_back();
        }
    }
    Iterator<T> begin() { return Iterator<T>(head); }
    Iterator<T> end() { return Iterator<T>(nullptr); }
};