#pragma once

#include <array>
#include <cstdint>

#include <flexrpc_proto/common.pb.h>

namespace flexrpc {

constexpr size_t MAX_PAYLOAD_BYTES = 1024;

class FlexPoint {
public:
    explicit FlexPoint();
    ~FlexPoint();
    std::string str_;

    void process();

private:
    std::array<uint8_t, MAX_PAYLOAD_BYTES> tx_buffer_;
    std::array<uint8_t, MAX_PAYLOAD_BYTES> rx_buffer_;
};

}  // namespace flexrpc
