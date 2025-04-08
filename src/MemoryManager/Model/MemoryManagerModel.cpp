#include "MemoryManagerModel.h"
#include <cstring>
#include <iostream>

MemoryManagerModel::MemoryManagerModel(size_t memorySize) 
    : memorySize(memorySize), nextId(1), gcRunning(false) {
    // Allocate the single large block of memory
    memory = malloc(memorySize);
    if (!memory) {
        throw std::runtime_error("Failed to allocate memory");
    }
    // Initialize memory to zeros
    memset(memory, 0, memorySize);
}

MemoryManagerModel::~MemoryManagerModel() {
    StopGarbageCollector();
    // Free the single allocated memory block
    free(memory);
}

int MemoryManagerModel::Create(size_t size, const std::string& type) {
    std::lock_guard<std::mutex> lock(memoryMutex);
    
    // Find free space in memory
    size_t offset = FindFreeSpace(size);
    if (offset == -1) {
        // No space available
        Defragment();
        offset = FindFreeSpace(size);
        if (offset == -1) {
            return -1; // Still no space after defragmentation
        }
    }
    
    // Create a new block
    MemoryBlock block;
    block.id = nextId++;
    block.offset = offset;
    block.size = size;
    block.type = type;
    block.refCount = 1; // Initial reference count
    block.isAllocated = true;
    
    allocatedBlocks.push_back(block);
    return block.id;
}

bool MemoryManagerModel::Set(int id, const void* value, size_t valueSize) {
    std::lock_guard<std::mutex> lock(memoryMutex);
    
    MemoryBlock* block = FindBlockById(id);
    if (!block || !block->isAllocated) {
        return false;
    }
    
    if (valueSize > block->size) {
        return false; // Value is too large for the block
    }
    
    // Copy the value to the memory block
    char* dest = static_cast<char*>(memory) + block->offset;
    memcpy(dest, value, valueSize);
    
    return true;
}

bool MemoryManagerModel::Get(int id, void* value, size_t maxSize, size_t& actualSize) {
    std::lock_guard<std::mutex> lock(memoryMutex);
    
    MemoryBlock* block = FindBlockById(id);
    if (!block || !block->isAllocated) {
        return false;
    }
    
    actualSize = std::min(maxSize, block->size);
    
    // Copy from the memory block to the provided buffer
    char* src = static_cast<char*>(memory) + block->offset;
    memcpy(value, src, actualSize);
    
    return true;
}

bool MemoryManagerModel::IncreaseRefCount(int id) {
    std::lock_guard<std::mutex> lock(memoryMutex);
    
    MemoryBlock* block = FindBlockById(id);
    if (!block || !block->isAllocated) {
        return false;
    }
    
    block->refCount++;
    return true;
}

bool MemoryManagerModel::DecreaseRefCount(int id) {
    std::lock_guard<std::mutex> lock(memoryMutex);
    
    MemoryBlock* block = FindBlockById(id);
    if (!block || !block->isAllocated) {
        return false;
    }
    
    block->refCount--;
    // Note: We don't free blocks here - the garbage collector will handle that
    return true;
}

void MemoryManagerModel::StartGarbageCollector() {
    if (!gcRunning) {
        gcRunning = true;
        gcThread = std::thread(&MemoryManagerModel::GarbageCollectorTask, this);
    }
}

void MemoryManagerModel::StopGarbageCollector() {
    if (gcRunning) {
        gcRunning = false;
        if (gcThread.joinable()) {
            gcThread.join();
        }
    }
}

void MemoryManagerModel::GarbageCollectorTask() {
    while (gcRunning) {
        {
            std::lock_guard<std::mutex> lock(memoryMutex);
            
            // Check for blocks with zero references
            auto it = allocatedBlocks.begin();
            while (it != allocatedBlocks.end()) {
                if (it->isAllocated && it->refCount <= 0) {
                    // Mark block as free
                    it->isAllocated = false;
                    // Zero out the memory
                    char* blockStart = static_cast<char*>(memory) + it->offset;
                    memset(blockStart, 0, it->size);
                }
                ++it;
            }
        }
        
        // Sleep before next collection cycle
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void MemoryManagerModel::Defragment() {
    // Remove unused blocks
    allocatedBlocks.erase(
        std::remove_if(allocatedBlocks.begin(), allocatedBlocks.end(),
            [](const MemoryBlock& block) { return !block.isAllocated; }),
        allocatedBlocks.end()
    );
    
    if (allocatedBlocks.empty()) {
        return; // Nothing to defragment
    }
    
    // Sort blocks by offset
    std::sort(allocatedBlocks.begin(), allocatedBlocks.end(),
        [](const MemoryBlock& a, const MemoryBlock& b) {
            return a.offset < b.offset;
        });
    
    // Move blocks to eliminate gaps
    size_t currentOffset = 0;
    for (auto& block : allocatedBlocks) {
        if (block.offset != currentOffset) {
            // Move memory
            char* src = static_cast<char*>(memory) + block.offset;
            char* dest = static_cast<char*>(memory) + currentOffset;
            memmove(dest, src, block.size);
            // Update offset
            block.offset = currentOffset;
        }
        currentOffset += block.size;
    }
}

MemoryBlock* MemoryManagerModel::FindBlockById(int id) {
    for (auto& block : allocatedBlocks) {
        if (block.id == id) {
            return &block;
        }
    }
    return nullptr;
}

size_t MemoryManagerModel::FindFreeSpace(size_t size) {
    if (allocatedBlocks.empty()) {
        // Memory is empty
        return 0;
    }
    
    // Sort blocks by offset
    std::sort(allocatedBlocks.begin(), allocatedBlocks.end(),
        [](const MemoryBlock& a, const MemoryBlock& b) {
            return a.offset < b.offset;
        });
    
    // Check for space before the first block
    if (allocatedBlocks.front().offset >= size) {
        return 0;
    }
    
    // Check for space between blocks
    for (size_t i = 0; i < allocatedBlocks.size() - 1; ++i) {
        const auto& current = allocatedBlocks[i];
        const auto& next = allocatedBlocks[i + 1];
        
        size_t endOfCurrent = current.offset + current.size;
        size_t availableSpace = next.offset - endOfCurrent;
        
        if (availableSpace >= size) {
            return endOfCurrent;
        }
    }
    
    // Check for space after the last block
    const auto& last = allocatedBlocks.back();
    size_t endOfLast = last.offset + last.size;
    if (memorySize - endOfLast >= size) {
        return endOfLast;
    }
    
    // No suitable space found
    return -1;
}

size_t MemoryManagerModel::GetTypeSize(const std::string& type) {
    if (type == "int") return sizeof(int);
    if (type == "float") return sizeof(float);
    if (type == "double") return sizeof(double);
    if (type == "char") return sizeof(char);
    if (type == "bool") return sizeof(bool);
    // Add more types as needed
    return 0; // Unknown type
}