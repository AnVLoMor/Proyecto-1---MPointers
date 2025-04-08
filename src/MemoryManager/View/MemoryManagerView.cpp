#include "MemoryManagerView.h"
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

MemoryManagerView::MemoryManagerView(const MemoryManagerModel* model, const std::string& dumpFolder)
    : model(model), dumpFolder(dumpFolder) {
    // Create dump folder if it doesn't exist
    if (!fs::exists(dumpFolder)) {
        fs::create_directories(dumpFolder);
    }
}

void MemoryManagerView::DisplayMemoryState() const {
    std::cout << "Memory Manager State:" << std::endl;
    std::cout << "Total Memory: " << model->GetMemorySize() << " bytes" << std::endl;
    
    const auto& blocks = model->GetAllocatedBlocks();
    std::cout << "Allocated Blocks: " << blocks.size() << std::endl;
    
    for (const auto& block : blocks) {
        std::cout << FormatMemoryBlock(block) << std::endl;
    }
}

void MemoryManagerView::GenerateDump() const {
    std::string timestamp = GenerateTimestamp();
    std::string filename = dumpFolder + "/memory_dump_" + timestamp + ".txt";
    
    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
        std::cerr << "Failed to create dump file: " << filename << std::endl;
        return;
    }
    
    outFile << "Memory Dump - " << timestamp << std::endl;
    outFile << "Total Memory: " << model->GetMemorySize() << " bytes" << std::endl << std::endl;
    
    const auto& blocks = model->GetAllocatedBlocks();
    outFile << "Allocated Blocks: " << blocks.size() << std::endl;
    
    for (const auto& block : blocks) {
        outFile << FormatMemoryBlock(block) << std::endl;
        
        // Dump block content as hex
        const char* memPtr = static_cast<const char*>(model->GetMemoryPointer()) + block.offset;
        outFile << "Content (hex): ";
        
        for (size_t i = 0; i < std::min(block.size, size_t(32)); ++i) {
            outFile << std::hex << std::setw(2) << std::setfill('0') 
                    << static_cast<int>(static_cast<unsigned char>(memPtr[i])) << " ";
            if ((i + 1) % 8 == 0) outFile << " ";
        }
        if (block.size > 32) outFile << "...";
        outFile << std::dec << std::endl << std::endl;
    }
    
    outFile.close();
    std::cout << "Memory dump created: " << filename << std::endl;
}

std::string MemoryManagerView::GenerateTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    auto timer = std::chrono::system_clock::to_time_t(now);
    
    std::tm bt;
    #ifdef _WIN32
    localtime_s(&bt, &timer);
    #else
    bt = *std::localtime(&timer);
    #endif
    
    std::ostringstream oss;
    oss << std::put_time(&bt, "%Y%m%d_%H%M%S") << "_" << std::setfill('0') << std::setw(3) << ms.count();
    return oss.str();
}

std::string MemoryManagerView::FormatMemoryBlock(const MemoryBlock& block) const {
    std::ostringstream oss;
    oss << "Block ID: " << block.id << " | "
        << "Offset: " << block.offset << " | "
        << "Size: " << block.size << " bytes | "
        << "Type: " << block.type << " | "
        << "RefCount: " << block.refCount << " | "
        << "Status: " << (block.isAllocated ? "Allocated" : "Free");
    return oss.str();
}