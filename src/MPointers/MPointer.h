#pragma once

#include "GRPCClient.h"
#include <iostream>
#include <string>
#include <typeinfo>
#include <cstring>

template <typename T>
class MPointer {
public:
    // Initialize connection to Memory Manager
    static void Init(int port) {
        std::string server_address = "localhost:" + std::to_string(port);
        if (!GRPCClient::getInstance().Connect(server_address)) {
            throw std::runtime_error("Failed to connect to Memory Manager");
        }
    }
    
    // Default constructor
    MPointer() : id(-1) {}
    
    // Create a new pointer (allocate memory)
    static MPointer<T> New() {
        MPointer<T> ptr;
        size_t size = sizeof(T);
        std::string type = typeid(T).name();
        
        ptr.id = GRPCClient::getInstance().Create(size, type);
        if (ptr.id == -1) {
            throw std::runtime_error("Failed to create memory block");
        }
        
        return ptr;
    }
    
    // Destructor
    ~MPointer() {
        if (id != -1) {
            GRPCClient::getInstance().DecreaseRefCount(id);
        }
    }
    
    // Copy constructor
    MPointer(const MPointer<T>& other) : id(other.id) {
        if (id != -1) {
            GRPCClient::getInstance().IncreaseRefCount(id);
        }
    }
    
    // Assignment operator
    MPointer<T>& operator=(const MPointer<T>& other) {
        if (this != &other) {
            // Decrease reference count for current id
            if (id != -1) {
                GRPCClient::getInstance().DecreaseRefCount(id);
            }
            
            // Copy id from other and increase reference count
            id = other.id;
            if (id != -1) {
                GRPCClient::getInstance().IncreaseRefCount(id);
            }
        }
        return *this;
    }
    
    // Dereference operator (for value access)
    T operator*() const {
        if (id == -1) {
            throw std::runtime_error("Dereferencing null MPointer");
        }
        
        T value;
        size_t actualSize = 0;
        bool success = GRPCClient::getInstance().Get(id, &value, sizeof(T), actualSize);
        
        if (!success || actualSize != sizeof(T)) {
            throw std::runtime_error("Failed to get value from memory");
        }
        
        return value;
    }
    
    // Reference assignment operator
    void operator=(const T& value) {
        if (id == -1) {
            throw std::runtime_error("Assigning to null MPointer");
        }
        
        bool success = GRPCClient::getInstance().Set(id, &value, sizeof(T));
        if (!success) {
            throw std::runtime_error("Failed to set value in memory");
        }
    }
    
    // Address-of operator (returns id)
    int operator&() const {
        return id;
    }
    
    // Check if pointer is valid
    bool isValid() const {
        return id != -1;
    }
    
private:
    int id; // Memory block ID in Memory Manager
};