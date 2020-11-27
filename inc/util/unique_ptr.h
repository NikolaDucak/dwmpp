#pragma once

namespace util {

/**
 * Utility pointer providing similar funcitonality as STL's std::unique_ptr<T>
 * but allowing copy construction, treating it as move
 */
template <typename T>
class unique_ptr {
public:
    unique_ptr() : m_ptr(nullptr) {}
    unique_ptr(unique_ptr& other) : m_ptr(nullptr) {}
private:
    T* m_ptr;
}

}  // namespace util
