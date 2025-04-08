#pragma once

#include "MPointer.h"
#include <stdexcept>

template <typename T>
class Node {
public:
    T data;
    MPointer<Node<T>> next;
    
    Node() {}
    Node(const T& value) : data(value) {}
};

template <typename T>
class LinkedList {
public:
    LinkedList() : size(0) {}
    
    // Add element to the end of the list
    void add(const T& data) {
        MPointer<Node<T>> newNode = MPointer<Node<T>>::New();
        *newNode = Node<T>(data);
        
        if (!head.isValid()) {
            head = newNode;
        } else {
            MPointer<Node<T>> current = head;
            while ((*current).next.isValid()) {
                current = (*current).next;
            }
            (*current).next = newNode;
        }
        
        size++;
    }
    
    // Remove element at specified index
    bool remove(int index) {
        if (index < 0 || index >= size) {
            return false;
        }
        
        if (index == 0) {
            // Remove first element
            if (head.isValid()) {
                MPointer<Node<T>> temp = head;
                head = (*temp).next;
                size--;
                return true;
            }
            return false;
        }
        
        // Find the node before the one to remove
        MPointer<Node<T>> current = head;
        for (int i = 0; i < index - 1; i++) {
            current = (*current).next;
        }
        
        // Remove the node
        MPointer<Node<T>> toRemove = (*current).next;
        (*current).next = (*toRemove).next;
        size--;
        
        return true;
    }
    
    // Get element at specified index
    T get(int index) const {
        if (index < 0 || index >= size) {
            throw std::out_of_range("Index out of range");
        }
        
        MPointer<Node<T>> current = head;
        for (int i = 0; i < index; i++) {
            current = (*current).next;
        }
        
        return (*current).data;
    }
    
    // Display all elements in the list
    void display() const {
        MPointer<Node<T>> current = head;
        std::cout << "List: ";
        while (current.isValid()) {
            std::cout << (*current).data << " ";
            current = (*current).next;
        }
        std::cout << std::endl;
    }
    
    // Get the size of the list
    int getSize() const {
        return size;
    }
    
    // Check if the list is empty
    bool isEmpty() const {
        return size == 0;
    }
    
private:
    MPointer<Node<T>> head;
    int size;
};