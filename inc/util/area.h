#pragma once

#include "util/point.h"

namespace util {

struct area {
    point    top_left;
    unsigned width, height;
};


inline std::ostream& operator<<(std::ostream& os, const util::area& a) {
    os << "area { " << a.top_left << ", " << a.width << ", " << a.height
       << " }";
    return os;
}

}  // namespace util
