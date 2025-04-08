#pragma once

#include <string>
#include <memory>
#include <grpcpp/grpcpp.h>
#include "mpointers.grpc.pb.h"

class GRPCClient {
public:
    static GRPCClient& getInstance();
    
    bool Connect(const std::string& server_address);
    void Disconnect();
    bool IsConnected() const;
    
    int Create(size_t size, const std::string& type);
    bool Set(int id, const void* value, size_t valueSize);
    bool Get(int id, void* value, size_t maxSize, size_t& actualSize);
    bool IncreaseRefCount(int id);
    bool DecreaseRefCount(int id);
    
private:
    GRPCClient();
    ~GRPCClient();
    
    // Prevent copy
    GRPCClient(const GRPCClient&) = delete;
    GRPCClient& operator=(const GRPCClient&) = delete;
    
    std::unique_ptr<mpointers::MemoryManager::Stub> stub_;
    std::shared_ptr<grpc::Channel> channel_;
    bool connected;
};