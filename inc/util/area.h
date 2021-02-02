#pragma once

#include "util/point.h"

namespace util {

struct rect {
    point    top_left;
    unsigned width, height;

    /**
     * Calculates the area of 2 overlaping areas, if the areas
     * are not overlaping, 0 is returned
     */
    inline unsigned intersection_area(const rect& other) const {
        using std::max, std::min;
        return max(0U, min(other.top_left.x + other.width,
                           this->top_left.x + this->width) -
                           max(this->top_left.x, other.top_left.x)) *
               max(0U, min(this->top_left.y + this->height,
                           other.top_left.y + other.height) -
                           max(this->top_left.y, other.top_left.y));
    }

    /**
     * Checks if point @p p is within area.
     */
    inline bool contains(const point& p) const {
        return top_left.x <= p.x &&
               p.x <= static_cast<int>(top_left.x + width) &&
               top_left.y <= p.y &&
               p.y <= static_cast<int>(top_left.y + height);
    }
};

inline std::ostream& operator<<(std::ostream& os, const util::rect& a) {
    os << "area { " << a.top_left << ", " << a.width << ", " << a.height
       << " }";
    return os;
}

}  // namespace util
