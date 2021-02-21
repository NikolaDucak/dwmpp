#pragma once 

#include <algorithm>

namespace util {

template<typename T>
auto restrict_to_range(T val ,T min, T max) {
    return std::min(std::max(min,val), max);
}

} // namespace util
