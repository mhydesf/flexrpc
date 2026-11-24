#pragma once

#include <flexregistry.h>
#include <flexservice.h>

namespace flexrpc {

class FlexPoint {
public:
    constexpr FlexPoint() {}
    ~FlexPoint();

    void process();

private:
    Registry registry_{};
};

}  // namespace flexrpc
