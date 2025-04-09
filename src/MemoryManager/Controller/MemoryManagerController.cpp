#include "MemoryManagerController.h"
#include <iostream>

MemoryManagerServiceImpl::MemoryManagerServiceImpl(MemoryManagerModel* model, MemoryManagerView* view)
    : model(model), view(view) {}

grpc::Status MemoryManagerServiceImpl::Create(grpc::ServerContext* context, 
                                        const mpointers::CreateRequest* request,
                                        mpointers::CreateResponse* response) {
    int id = model->Create(request->size(), request->type());
    
    response->set_id(id);
    response->set_success(id != -1);
    if (id == -1) {
        response->set_error_message("Failed to allocate memory block");
    }
    
    // Generate memory dump after modifying memory
    view->GenerateDump();
    
    return grpc::Status::OK;
}

grpc::Status MemoryManagerServiceImpl::Set(grpc::ServerContext* context, 
                                    const mpointers::SetRequest* request,
                                    mpointers::SetResponse* response) {
    bool success = model->Set(request->id(), request->value().data(), request->value().size());
    
    response->set_success(success);
    if (!success) {
        response->set_error_message("Failed to set value in memory block");
    }
    
    // Generate memory dump after modifying memory
    view->GenerateDump();
    
    return grpc::Status::OK;
}

grpc::Status MemoryManagerServiceImpl::Get(grpc::ServerContext* context, 
                                    const mpointers::GetRequest* request,
                                    mpointers::GetResponse* response) {
    // We need a temporary buffer - we'll use 1MB as a reasonable max size
    const size_t MAX_SIZE = 1024 * 1024;
    std::vector<char> buffer(MAX_SIZE);
    size_t actualSize = 0;
    
    bool success = model->Get(request->id(), buffer.data(), MAX_SIZE, actualSize);
    
    response->set_success(success);
    if (success) {
        response->set_value(buffer.data(), actualSize);
    } else {
        response->set_error_message("Failed to get value from memory block");
    }
    
    return grpc::Status::OK;
}

grpc::Status MemoryManagerServiceImpl::IncreaseRefCount(grpc::ServerContext* context, 
                                                const mpointers::RefCountRequest* request,
                                                mpointers::RefCountResponse* response) {
    bool success = model->IncreaseRefCount(request->id());
    
    response->set_success(success);
    if (!success) {
        response->set_error_message("Failed to increase reference count");
    }
    
    return grpc::Status::OK;
}

grpc::Status MemoryManagerServiceImpl::DecreaseRefCount(grpc::ServerContext* context, 
                                                const mpointers::RefCountRequest* request,
                                                mpointers::RefCountResponse* response) {
    bool success = model->DecreaseRefCount(request->id());
    
    response->set_success(success);
    if (!success) {
        response->set_error_message("Failed to decrease reference count");
    }
    
    // Generate memory dump after potentially modifying memory state
    view->GenerateDump();
    
    return grpc::Status::OK;
}

MemoryManagerController::MemoryManagerController(int port, size_t memorySize, const std::string& dumpFolder)
    : port(port) {
    
    // Create model and view
    model = std::make_unique<MemoryManagerModel>(memorySize);
    view = std::make_unique<MemoryManagerView>(model.get(), dumpFolder);
    
    // Start garbage collector
    model->StartGarbageCollector();
    
    // Create service
    service = std::make_unique<MemoryManagerServiceImpl>(model.get(), view.get());
}

MemoryManagerController::~MemoryManagerController() {
    Stop();
}

void MemoryManagerController::Start() {
    std::string serverAddress = "0.0.0.0:" + std::to_string(port);
    
    grpc::ServerBuilder builder;
    builder.AddListeningPort(serverAddress, grpc::InsecureServerCredentials());
    builder.RegisterService(service.get());
    
    // Build and start server
    server = builder.BuildAndStart();
    std::cout << "Memory Manager listening on " << serverAddress << std::endl;
    
    view->DisplayMemoryState();
    
    // Block until server is shutdown
    server->Wait();
}

void MemoryManagerController::Stop() {
    if (server) {
        server->Shutdown();
    }
    
    model->StopGarbageCollector();
}