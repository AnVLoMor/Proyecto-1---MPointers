#include "GRPCClient.h"
#include <iostream>

GRPCClient& GRPCClient::getInstance() {
    static GRPCClient instance;
    return instance;
}

GRPCClient::GRPCClient() : connected(false) {}

GRPCClient::~GRPCClient() {
    Disconnect();
}

bool GRPCClient::Connect(const std::string& server_address) {
    if (connected) {
        return true; // Already connected
    }
    
    channel_ = grpc::CreateChannel(server_address, grpc::InsecureChannelCredentials());
    stub_ = mpointers::MemoryManager::NewStub(channel_);
    
    // Try a simple ping to check connection
    grpc::ClientContext context;
    mpointers::RefCountRequest request;
    mpointers::RefCountResponse response;
    request.set_id(-1); // Invalid ID for a ping
    
    grpc::Status status = stub_->IncreaseRefCount(&context, request, &response);
    connected = status.error_code() != grpc::StatusCode::UNAVAILABLE;
    
    if (!connected) {
        std::cerr << "Failed to connect to Memory Manager at " << server_address << std::endl;
        std::cerr << "Error: " << status.error_message() << std::endl;
    } else {
        std::cout << "Connected to Memory Manager at " << server_address << std::endl;
    }
    
    return connected;
}

void GRPCClient::Disconnect() {
    stub_.reset();
    channel_.reset();
    connected = false;
}

bool GRPCClient::IsConnected() const {
    return connected;
}

int GRPCClient::Create(size_t size, const std::string& type) {
    if (!connected) {
        std::cerr << "Not connected to Memory Manager" << std::endl;
        return -1;
    }
    
    grpc::ClientContext context;
    mpointers::CreateRequest request;
    mpointers::CreateResponse response;
    
    request.set_size(size);
    request.set_type(type);
    
    grpc::Status status = stub_->Create(&context, request, &response);
    
    if (!status.ok()) {
        std::cerr << "Error creating memory block: " << status.error_message() << std::endl;
        return -1;
    }
    
    if (!response.success()) {
        std::cerr << "Failed to create memory block: " << response.error_message() << std::endl;
        return -1;
    }
    
    return response.id();
}

bool GRPCClient::Set(int id, const void* value, size_t valueSize) {
    if (!connected) {
        std::cerr << "Not connected to Memory Manager" << std::endl;
        return false;
    }
    
    grpc::ClientContext context;
    mpointers::SetRequest request;
    mpointers::SetResponse response;
    
    request.set_id(id);
    request.set_value(value, valueSize);
    
    grpc::Status status = stub_->Set(&context, request, &response);
    
    if (!status.ok()) {
        std::cerr << "Error setting value: " << status.error_message() << std::endl;
        return false;
    }
    
    if (!response.success()) {
        std::cerr << "Failed to set value: " << response.error_message() << std::endl;
        return false;
    }
    
    return true;
}

bool GRPCClient::Get(int id, void* value, size_t maxSize, size_t& actualSize) {
    if (!connected) {
        std::cerr << "Not connected to Memory Manager" << std::endl;
        return false;
    }
    
    grpc::ClientContext context;
    mpointers::GetRequest request;
    mpointers::GetResponse response;
    
    request.set_id(id);
    
    grpc::Status status = stub_->Get(&context, request, &response);
    
    if (!status.ok()) {
        std::cerr << "Error getting value: " << status.error_message() << std::endl;
        return false;
    }
    
    if (!response.success()) {
        std::cerr << "Failed to get value: " << response.error_message() << std::endl;
        return false;
    }
    
    const std::string& data = response.value();
    actualSize = std::min(maxSize, data.size());
    memcpy(value, data.data(), actualSize);
    
    return true;
}

bool GRPCClient::IncreaseRefCount(int id) {
    if (!connected) {
        std::cerr << "Not connected to Memory Manager" << std::endl;
        return false;
    }
    
    grpc::ClientContext context;
    mpointers::RefCountRequest request;
    mpointers::RefCountResponse response;
    
    request.set_id(id);
    
    grpc::Status status = stub_->IncreaseRefCount(&context, request, &response);
    
    if (!status.ok()) {
        std::cerr << "Error increasing reference count: " << status.error_message() << std::endl;
        return false;
    }
    
    if (!response.success()) {
        std::cerr << "Failed to increase reference count: " << response.error_message() << std::endl;
        return false;
    }
    
    return true;
}

bool GRPCClient::DecreaseRefCount(int id) {
    if (!connected) {
        std::cerr << "Not connected to Memory Manager" << std::endl;
        return false;
    }
    
    grpc::ClientContext context;
    mpointers::RefCountRequest request;
    mpointers::RefCountResponse response;
    
    request.set_id(id);
    
    grpc::Status status = stub_->DecreaseRefCount(&context, request, &response);
    
    if (!status.ok()) {
        std::cerr << "Error decreasing reference count: " << status.error_message() << std::endl;
        return false;
    }
    
    if (!response.success()) {
        std::cerr << "Failed to decrease reference count: " << response.error_message() << std::endl;
        return false;
    }
    
    return true;
}