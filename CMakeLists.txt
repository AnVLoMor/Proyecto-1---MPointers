cmake_minimum_required(VERSION 3.10)
project(MPointers2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find packages
find_package(Protobuf REQUIRED)
find_package(gRPC REQUIRED)
find_package(Qt5 COMPONENTS Widgets REQUIRED)

# Generate protobuf code
get_filename_component(proto_file "proto/mpointers.proto" ABSOLUTE)
get_filename_component(proto_path "${proto_file}" PATH)

set(proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/mpointers.pb.cc")
set(proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/mpointers.pb.h")
set(grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/mpointers.grpc.pb.cc")
set(grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/mpointers.grpc.pb.h")

add_custom_command(
      OUTPUT "${proto_srcs}" "${proto_hdrs}" "${grpc_srcs}" "${grpc_hdrs}"
      COMMAND ${PROTOBUF_PROTOC_EXECUTABLE}
      ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${proto_path}"
        --plugin=protoc-gen-grpc="${GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${proto_file}"
      DEPENDS "${proto_file}")

# Include generated *.pb.h files
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

# Memory Manager executable
add_executable(mem-mgr
    src/MemoryManager/main.cpp
    src/MemoryManager/Model/MemoryManagerModel.cpp
    src/MemoryManager/View/MemoryManagerView.cpp
    src/MemoryManager/Controller/MemoryManagerController.cpp
    ${proto_srcs}
    ${grpc_srcs})

target_link_libraries(mem-mgr
    ${PROTOBUF_LIBRARIES}
    gRPC::grpc++
    gRPC::grpc++_reflection
    pthread)

# MPointers Test Client executable with UI
add_executable(mpointers-client
    src/MPointers/GRPCClient.cpp
    src/UI/MainWindow.cpp
    ${proto_srcs}
    ${grpc_srcs})

target_link_libraries(mpointers-client
    ${PROTOBUF_LIBRARIES}
    gRPC::grpc++
    Qt5::Widgets
    pthread)

target_include_directories(mem-mgr PRIVATE
    src/MemoryManager/Model
    src/MemoryManager/View
    src/MemoryManager/Controller)

target_include_directories(mpointers-client PRIVATE
    src/MPointers
    src/UI)