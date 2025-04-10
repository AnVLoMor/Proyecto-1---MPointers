#include "Controller/MemoryManagerController.h"
#include <iostream>
#include <string>
#include <cstdlib>

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " --port PORT --memsize SIZE_MB --dumpFolder FOLDER" << std::endl;
    std::cout << "  PORT: Port to listen on" << std::endl;
    std::cout << "  SIZE_MB: Size of memory to allocate in megabytes" << std::endl;
    std::cout << "  FOLDER: Folder to store memory dumps" << std::endl;
}

int main(int argc, char** argv) {
    int port = 50051;
    size_t memorySize = 100 * 1024 * 1024; // Default: 100MB
    std::string dumpFolder = "./dumps";
    
    // Parse command line arguments
    for (int i = 1; i < argc; i += 2) {
        std::string arg = argv[i];
        
        if (i + 1 >= argc && arg != "--help") {
            std::cerr << "Missing value for argument: " << arg << std::endl;
            printUsage(argv[0]);
            return 1;
        }
        
        if (arg == "--port") {
            port = std::atoi(argv[i + 1]);
        } else if (arg == "--memsize") {
            int sizeMB = std::atoi(argv[i + 1]);
            memorySize = static_cast<size_t>(sizeMB) * 1024 * 1024;
        } else if (arg == "--dumpFolder") {
            dumpFolder = argv[i + 1];
        } else if (arg == "--help") {
            printUsage(argv[0]);
            return 0;
        } else {
            std::cerr << "Unknown argument: " << arg << std::endl;
            printUsage(argv[0]);
            return 1;
        }
    }
    
    std::cout << "Starting Memory Manager..." << std::endl;
    std::cout << "  Port: " << port << std::endl;
    std::cout << "  Memory Size: " << (memorySize / (1024 * 1024)) << "MB" << std::endl;
    std::cout << "  Dump Folder: " << dumpFolder << std::endl;
    
    try {
        MemoryManagerController controller(port, memorySize, dumpFolder);
        controller.Start();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
</antA