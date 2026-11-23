#pragma once

#include <cstdio>
#include <flexregistry.h>

namespace flexrpc {

class ServiceBase {
public:
    constexpr ServiceBase(Registry& registry) : registry_{registry} {}

    ~ServiceBase() {
        // inform registry service EOL
        registry_.unregister_service(this);
    }

protected:
    template <auto Fn, typename Self>
    constexpr void bind(this Self& self, uint32_t id) {
        self.registry_.register_method(FlexMethod::bind<Fn, Self>(id, &self));
    }

private:
    Registry& registry_;
};

class SystemService : public ServiceBase {
public:
    constexpr SystemService(Registry& registry) : ServiceBase{registry} {}

    constexpr ssize_t handle_discover(std::span<const uint8_t> req, std::span<uint8_t> resp) {
        printf("discover\n");
        return 0;
    }

    constexpr ssize_t handle_capabilities(std::span<const uint8_t> req, std::span<uint8_t> resp) {
        printf("capabilites\n");
        return 0;
    }

    constexpr void init() {
        bind<&SystemService::handle_discover>(0);
        bind<&SystemService::handle_capabilities>(1);
    }
};

}  // namespace flexrpc
