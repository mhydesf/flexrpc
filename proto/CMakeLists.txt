set(PROTO_GEN_DIR "${CMAKE_CURRENT_BINARY_DIR}/flexrpc_proto")
file(MAKE_DIRECTORY "${PROTO_GEN_DIR}")

add_library(flexrpc-proto STATIC
    common.proto
)

target_link_libraries(
    flexrpc-proto
    PUBLIC
        protobuf::libprotobuf
    )
target_include_directories(
    flexrpc-proto
    PUBLIC
        "${CMAKE_CURRENT_BINARY_DIR}/"
    )

protobuf_generate(
    TARGET flexrpc-proto
    PROTOC_EXE "${FLEXRPC_PROTOC_EXECUTABLE}"
    IMPORT_DIRS "${CMAKE_CURRENT_SOURCE_DIR}"
    PROTOC_OUT_DIR "${CMAKE_BINARY_DIR}/flexrpc_proto/flexrpc_proto" 
)
