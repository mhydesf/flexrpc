#pragma once

#include <cstdint>

#include <flexmethod.h>

namespace flexrpc {

class Registry {
public:
    constexpr Registry() = default;

    constexpr void register_method(FlexMethod method) {
        if (method.id() >= MAX_REGISTRY_METHODS) { return; }
        auto& entry = registry_.at(method.id());
        // if already valid don't overwrite entry
        if (entry.is_valid()) { return; }
        entry.method = method;
        entry.state = EntryState::VALID;
    }

    constexpr void register_methods(std::initializer_list<FlexMethod> methods) {
        for (const auto& m : methods) { register_method(m); }
    }

    void unregister_service(void* service) {
        for (auto& entry : registry_) {
            if(entry.method.service() == service) {
                entry.method = FlexMethod{};
                entry.state = EntryState::STALE;
            }
        }
    }

    ssize_t invoke(uint32_t id, std::span<const uint8_t> req, std::span<uint8_t> resp) {
        if (id >= MAX_REGISTRY_METHODS) { return -1; }
        auto& entry = registry_.at(id);
        if (!entry.is_valid()) { return -2; }
        return registry_.at(id).method.invoke(req, resp);
    }

private:
    [[nodiscard]] bool is_valid_method(uint32_t id) const {
        return registry_.at(id).is_valid();
    }

private:
    static constexpr uint32_t MAX_REGISTRY_METHODS = 512;

    enum class EntryState : uint8_t {
        UNINITIALIZED = 0,
        VALID,
        STALE
    };

    struct Entry {
        FlexMethod method;
        EntryState state;

        constexpr bool is_valid() const { return state == EntryState::VALID; }
    };

private:
    std::array<Entry, MAX_REGISTRY_METHODS> registry_{};
};

}  // namespace flexrpc
