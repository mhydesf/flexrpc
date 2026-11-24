#pragma once

#include <unistd.h>

#include <cstdint>
#include <functional>
#include <span>

namespace flexrpc {

// FlexMethods are free functions or members of a FlexServiceBase
// derived class
class FlexServiceBase;

template <typename T>
concept ServiceT = std::derived_from<T, FlexServiceBase>;

template <typename FnT, typename Req, typename Resp>
concept FreeHandlerT = requires(FnT fn, const Req& req, Resp& resp) {
    { std::invoke(fn, req, resp) } -> std::same_as<ssize_t>;
};

// TTP for ServiceT handlers
// requires the method is of appropriate shape and belongs to an instance derived from FlexServiceBase
template <typename FnT, typename Req, typename Resp, typename S>
concept SrvHandlerT = ServiceT<S> && requires(FnT fn, S* srv, const Req& req, Resp& resp) {
    { std::invoke(fn, srv, req, resp) } -> std::same_as<ssize_t>;
};

class FlexMethod {
public:
    constexpr uint32_t id() const { return id_; }
    constexpr ssize_t invoke(std::span<const uint8_t> req, std::span<uint8_t> resp) { return method_(service_, req, resp); }
    constexpr void* service() const { return service_; }

    // bind methods from a class derived from FlexServiceBase
    template <typename Req, typename Resp, ServiceT S, SrvHandlerT<Req, Resp, S> auto Fn>
    static constexpr FlexMethod bind(uint32_t id, S* srv) {
        return FlexMethod{{
            .id = id,
            .service = srv,
            .method = [](void* srv, std::span<const uint8_t> req_raw, std::span<uint8_t> resp_raw) -> ssize_t {
                return executive<Req, Resp>(req_raw, resp_raw, [srv](const Req& req, Resp& resp) {
                    return (static_cast<S*>(srv) ->*Fn)(req, resp);
                });
            }}
        };
    }

    // bind free methods like static functions or generic lambdas
    template <typename Req, typename Resp, FreeHandlerT<Req, Resp> auto Fn>
    static constexpr FlexMethod bind(uint32_t id) {
        return FlexMethod{{
            .id = id,
            .service = nullptr,
            .method = [](void*, std::span<const uint8_t> req_raw, std::span<uint8_t> resp_raw) -> ssize_t {
                return executive<Req, Resp>(req_raw, resp_raw, [](const Req& req, Resp& resp) {
                    return Fn(req, resp);
                });
            }}
        };
    }

    // TODO: delete this
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

    template <typename Req, typename Resp, typename CallFn>
    static constexpr ssize_t executive(std::span<const uint8_t> req_raw, std::span<uint8_t> resp_raw, CallFn call) {
        Req req{};
        if (!req.ParseFromArray(req_raw.data(), req_raw.size())) {
            return -1;
        }

        Resp resp{};
        if (!call(req, resp)) {
            return -2;
        }

        const size_t out_len = resp.ByteSizeLong();
        if (out_len > resp_raw.size() || !resp.SerializeToArray(resp_raw.data(), out_len)) {
            return -3;
        }

        return static_cast<ssize_t>(out_len);
    }

private:
    uint32_t id_{0};
    void* service_{nullptr};
    HandlerFn method_{nullptr};
};

}  // namesapce flexrpc
