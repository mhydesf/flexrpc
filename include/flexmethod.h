#pragma once

#include <unistd.h>

#include <cstdint>
#include <functional>
#include <span>

namespace flexrpc {

class ServiceBase;

// TTP for derived ServiceBase types
template <typename T>
concept ServiceT = std::derived_from<T, ServiceBase>;

// TTP for free handlers
// requires the method is of appropriate shape
template <typename FnT>
concept FreeHandlerT = requires(FnT fn, std::span<const uint8_t> req, std::span<uint8_t> resp) {
    { std::invoke(fn, req, resp) } -> std::same_as<ssize_t>;
};

// TTP for ServiceT handlers
// requires the method is of appropriate shape and belongs to an instance derived from ServiceBase
template <typename FnT, typename S>
concept SrvHandlerT = ServiceT<S> && requires(FnT fn, S* srv, std::span<const uint8_t> req, std::span<uint8_t> resp) {
    { std::invoke(fn, srv, req, resp) } -> std::same_as<ssize_t>;
};

class FlexMethod {
public:
    constexpr uint32_t id() const { return id_; }
    constexpr ssize_t invoke(std::span<const uint8_t> req, std::span<uint8_t> resp) { return method_(service_, req, resp); }
    constexpr void* service() const { return service_; }

    // TODO: ewww - decltype
    template <auto Fn, ServiceT S> requires SrvHandlerT<decltype(Fn), S>
    static constexpr FlexMethod bind(uint32_t id, S* srv) {
        return FlexMethod{{
            .id = id,
            .service = srv,
            .method = [](void* srv, std::span<const uint8_t> req, std::span<uint8_t> resp) -> ssize_t {
                return (static_cast<S*>(srv)->*Fn)(req, resp);
            }}
        };
    }

    template <FreeHandlerT auto Fn>
    static constexpr FlexMethod bind(uint32_t id) {
        return FlexMethod{{
            .id = id,
            .service = nullptr,
            .method = [](void*, std::span<const uint8_t> req, std::span<uint8_t> resp) -> ssize_t {
                return Fn(req, resp);
            }}
        };
    }

    constexpr FlexMethod() = default;

private:
    using HandlerFn = ssize_t (*)(void*, std::span<const uint8_t>, std::span<uint8_t>);

    struct MethodParams {
        uint32_t id;
        void* service;
        HandlerFn method;
    };

    constexpr FlexMethod(MethodParams params)
        : id_{params.id}
        , service_{params.service}
        , method_{params.method} {}

private:
    uint32_t id_{0};
    void* service_{nullptr};
    HandlerFn method_{nullptr};
};

}  // namesapce flexrpc
