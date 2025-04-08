#pragma once

#include "MemoryManagerModel.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <chrono>

class MemoryManagerView {
public:
    MemoryManagerView(const MemoryManagerModel* model, const std::string& dumpFolder);
    
    void DisplayMemoryState() const;
    void GenerateDump() const;
    
private:
    const MemoryManagerModel* model;
    std::string dumpFolder;
    
    std::string GenerateTimestamp() const;
    std::string FormatMemoryBlock(const MemoryBlock& block) const;
};