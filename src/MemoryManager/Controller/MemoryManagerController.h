#pragma once

#include "../Model/MemoryManagerModel.h"
#include "../View/MemoryManagerView.h"
#include <memory>
#include <string>
#include <grpcpp/grpcpp.h>
#include "mpointers.grpc.pb.h"

class MemoryManagerServiceImpl final : public mpointers::MemoryManager::Service {
public:
    explicit MemoryManagerServiceImpl(MemoryManagerModel* model, MemoryManagerView* view);
    
    virtual grpc::Status Create(grpc::ServerContext* context, 
                              const mpointers::CreateRequest* request,
                              mpointers::CreateResponse* response) override;
                              
    virtual grpc::Status Set(grpc::ServerContext* context, 
                           const mpointers::SetRequest* request,
                           mpointers::SetResponse* response) override;
                           
    virtual grpc::Status Get(grpc::ServerContext* context, 
                           const mpointers::GetRequest* request,
                           mpointers::GetResponse* response) override;
                           
    virtual grpc::Status IncreaseRefCount(grpc::ServerContext* context, 
                                       const mpointers::RefCountRequest* request,
                                       mpointers::RefCountResponse* response) override;
                                       
    virtual grpc::Status DecreaseRefCount(grpc::ServerContext* context, 
                                       const mpointers::RefCountRequest* request,
                                       mpointers::RefCountResponse* response) override;
private:
    MemoryManagerModel* model;
    MemoryManagerView* view;
};

class MemoryManagerController {
public:
    MemoryManagerController(int port, size_t memorySize, const std::string& dumpFolder);
    ~MemoryManagerController();
    
    void Start();
    void Stop();
    
private:
    int port;
    std::unique_ptr<MemoryManagerModel> model;
    std::unique_ptr<MemoryManagerView> view;
    std::unique_ptr<grpc::Server> server;
    std::unique_ptr<MemoryManagerServiceImpl> service;
};