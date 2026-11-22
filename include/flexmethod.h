#pragma once

#include <unistd.h>

#include <cstdint>
#include <span>
#include <functional>

namespace flexrpc {

class ServiceBase;

// TTP for free handlers
template <typename FnT>
concept FreeHandlerT = requires(FnT fn, std::span<const uint8_t> req, std::span<uint8_t> resp) {
    { std::invoke(fn, req, resp) } -> std::same_as<ssize_t>;
};

// TTP for derived ServiceBase types
template <typename T>
concept ServiceT = std::derived_from<T, ServiceBase>;

// TTP for ServiceT handlers
template <typename FnT, typename S>
concept SrvHandlerT = ServiceT<S> && requires(FnT fn, S* srv, std::span<const uint8_t> req, std::span<uint8_t> resp) {
    { std::invoke(fn, srv, req, resp) } -> std::same_as<ssize_t>;
};

class FlexMethod {
public:
    // Bind ServiceBase::Method to flex method
    // TODO: ewww - decltype
    template <auto Fn, ServiceT S> requires SrvHandlerT<decltype(Fn), S>
    static constexpr FlexMethod bind(uint32_t id, S* srv) {
        return FlexMethod{
            id,
            srv,
            [](ServiceBase* srv, std::span<const uint8_t> req, std::span<uint8_t> resp) -> ssize_t {
                return (static_cast<S*>(srv)->*Fn)(req, resp);
            }
        };
    }

    // Bind ::Method to flex method
    template <FreeHandlerT auto Fn>
    static constexpr FlexMethod bind(uint32_t id) {
        return FlexMethod{
            id,
            nullptr,
            [](ServiceBase*, std::span<const uint8_t> req, std::span<uint8_t> resp) -> ssize_t {
                return Fn(req, resp);
            }
        };
    }

private:
    using HandlerFn = ssize_t (*)(ServiceBase*, std::span<const uint8_t>, std::span<uint8_t>);

    constexpr FlexMethod(uint32_t id, ServiceBase* srv, HandlerFn method)
        : id_{id}
        , instance_{srv}
        , method_{method} {}

private:
    uint32_t id_{0};
    ServiceBase* instance_{nullptr};
    HandlerFn method_{nullptr};
};

}  // namesapce flexrpc
