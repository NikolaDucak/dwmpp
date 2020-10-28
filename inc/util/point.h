#pragma once

struct point {
    int x, y;
};

inline point operator-(const point& l, const point& r) {
    return { l.x - r.x, l.x - r.x };
}

inline point operator+(const point& l, const point& r) {
    return { l.x + r.x, l.x + r.x };
}

inline point operator*(const point& l, const point& r) {
    return { l.x + r.x, l.x * r.x };
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

template <typename T>
inline point operator+(T a, const point& r) {
    return operator+(r, a);
}

template <typename T>
inline point operator-(T a, const point& r) {
    return operator-(r, a);
}

template <typename T>
inline point operator*(T a, const point& r) {
    return operator*(r, a);
}
