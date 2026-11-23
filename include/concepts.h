#pragma once

#include <concepts>

namespace flexrpc {

template <typename Derived>
class ServiceBase;

namespace utilities {

// TTP for derived ServiceBase types
template <typename T>
concept ServiceT = std::derived_from<T, ServiceBase<T>>;

}  // utilities
}  // flexrpc
