set(PROTOBUF_VERSION "36.2")

CPMAddPackage(
    NAME protoc_host
    URL "https://github.com/protocolbuffers/protobuf/releases/download/v${PROTOBUF_VERSION}/protoc-${PROTOBUF_VERSION}-linux-x86_64.zip"
    DOWNLOAD_ONLY YES
)
set(FLEXRPC_PROTOC_EXECUTABLE "${protoc_host_SOURCE_DIR}/bin/protoc"
    CACHE FILEPATH "Host protoc binary")
file(CHMOD "${FLEXRPC_PROTOC_EXECUTABLE}"
    PERMISSIONS
    OWNER_READ
    OWNER_WRITE
    OWNER_EXECUTE
    GROUP_READ
    GROUP_EXECUTE
    WORLD_READ
    WORLD_EXECUTE
)

CPMAddPackage(
    NAME absl
    GIT_REPOSITORY https://github.com/abseil/abseil-cpp.git
    GIT_TAG 20250512.1
    OPTIONS
        "ABSL_PROPAGATE_CXX_STD ON"
        "ABSL_ENABLE_INSTALL OFF"
)

CPMAddPackage(
    NAME protobuf
    GIT_REPOSITORY https://github.com/protocolbuffers/protobuf.git
    GIT_TAG "v${PROTOBUF_VERSION}"
    OPTIONS
        "protobuf_BUILD_TESTS OFF"
        "protobuf_BUILD_CONFORMANCE OFF"
        "protobuf_BUILD_EXAMPLES OFF"
        "protobuf BUILD_PROTOC_BINARIES OFF"
        "protobuf_INSTALL OFF"
)

include(${protobuf_SOURCE_DIR}/cmake/protobuf-generate.cmake)
