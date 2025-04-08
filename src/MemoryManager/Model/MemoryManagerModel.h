#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <mutex>
#include <thread>
#include <chrono>
#include <algorithm>
#include <string>

struct MemoryBlock {
    int id;
    size_t offset;
    size_t size;
    std::string type;
    int refCount;
    bool isAllocated;
};

class MemoryManagerModel {
public:
    MemoryManagerModel(size_t memorySize);
    ~MemoryManagerModel();
    
    int Create(size_t size, const std::string& type);
    bool Set(int id, const void* value, size_t valueSize);
    bool Get(int id, void* value, size_t maxSize, size_t& actualSize);
    bool IncreaseRefCount(int id);
    bool DecreaseRefCount(int id);
    
    // For the view to access
    const void* GetMemoryPointer() const { return memory; }
    size_t GetMemorySize() const { return memorySize; }
    const std::vector<MemoryBlock>& GetAllocatedBlocks() const { return allocatedBlocks; }
    
    // Start garbage collector in a separate thread
    void StartGarbageCollector();
    void StopGarbageCollector();
    
    // Memory defragmentation
    void Defragment();

private:
    void* memory;
    size_t memorySize;
    std::vector<MemoryBlock> allocatedBlocks;
    std::mutex memoryMutex;
    
    int nextId;
    bool gcRunning;
    std::thread gcThread;
    
    void GarbageCollectorTask();
    MemoryBlock* FindBlockById(int id);
    size_t FindFreeSpace(size_t size);
    size_t GetTypeSize(const std::string& type);
};