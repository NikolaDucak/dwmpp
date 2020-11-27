#pragma once
namespace util {

struct point {
    int x, y;
};

inline point operator-(const point& l, const point& r) {
    return { l.x - r.x, l.y - r.y };
}

inline point operator+(const point& l, const point& r) {
    return { l.x + r.x, l.y + r.y };
}

inline point operator*(const point& l, const point& r) {
    return { l.x + r.x, l.y * r.y };
}

template <typename T>
inline point operator+(const point& l, T a) {
    return { l.x + a, l.y + a };
}

template <typename T>
inline point operator-(const point& l, T a) {
    return { l.x - a, l.y - a };
}

template <typename T>
inline point operator*(const point& l, T a) {
    return { l.x * a, l.y * a };
}

}
