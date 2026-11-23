#pragma once

#include <concepts.h>
#include <flexregistry.h>

namespace flexrpc {

template <typename Derived>
class ServiceBase {
public:
    constexpr ServiceBase(Registry& registry) : registry_{registry} {
        bind<&ServiceBase::callback>(0);
    }

    ~ServiceBase() {
        // inform registry service EOL
        registry_.unregister_service(this);
    }

    ssize_t callback(std::span<const uint8_t>, std::span<uint8_t>) { return 0; }

protected:
    template <auto Fn>
    void bind(uint32_t id) {
        registry_.register_method(FlexMethod::bind<Fn, Derived>(id, static_cast<Derived*>(this)));
    }

private:
    Registry& registry_;
};

}  // namespace flexrpc
