#pragma once

#include <cstdio>
#include <flexmethod.h>
#include <flexrpc_proto/common.pb.h>

namespace flexrpc {

class FlexServiceBase {
private:
    static constexpr uint32_t MAX_SERVICE_METHODS = 512;

public:
    constexpr FlexServiceBase() {}
    ~FlexServiceBase() {}
};

class SystemService : public FlexServiceBase {
public:
    constexpr SystemService() {}

    constexpr ssize_t handle_discover(const Header& req, Header& resp) {
        printf("discover\n");
        return 0;
    }

    constexpr ssize_t handle_capabilities(std::span<const uint8_t> req, std::span<uint8_t> resp) {
        printf("capabilites\n");
        return 0;
    }
};

}  // namespace flexrpc
