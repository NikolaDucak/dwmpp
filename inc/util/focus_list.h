#pragma once

#include <initializer_list>
#include <list>

namespace util {

template <typename T>
class focus_list : public std::list<T> {
    using list_iter = typename std::list<T>::iterator;
    list_iter m_focused_item;

public:
    focus_list() : m_focused_item(this->begin()) {}
    focus_list(std::initializer_list<T>& l) : std::list<T>(l) { }

    list_iter& focused() { return m_focused_item; }
    const list_iter& focused() const { return m_focused_item; }


    void focus_front() { m_focused_item = this->begin(); }

    list_iter circulate_next_with_end(int i) {
        auto focused_item = m_focused_item;
        if (i > 0) {
            while (i-- > 0) {
                if (focused_item == this->end())
                    focused_item = this->begin();
                else
                    focused_item++;
            }
        } else if (i < 0) {
            while (i++ < 0) {
                if (focused_item == this->begin())
                    focused_item = this->end();
                else
                    focused_item--;
            }
        }
        return focused_item;
    }

    list_iter circulate_next(int i) {
        auto focused_item = m_focused_item;
        // assume its not already on the end
        if (i > 0) {
            while (i--) {
                focused_item++;
                if (focused_item == this->end())
                    focused_item = this->begin();
            }
        } else if (i < 0) {
            while (i++) {
                if (focused_item == this->begin() ||
                    focused_item == this->end())
                    focused_item = --this->end();
                else
                    focused_item--;
            }
        }
        return focused_item;
    }

    void circulate_focus(int i) {
        // assume its not already on the end
        if (i > 0) {
            while (i--) {
                m_focused_item++;
                if (m_focused_item == this->end())
                    m_focused_item = this->begin();
            }
        } else if (i < 0) {
            while (i++) {
                if (m_focused_item == this->begin() ||
                    m_focused_item == this->end())
                    m_focused_item = --this->end();
                else
                    m_focused_item--;
            }
        }
    }

    void circulate_focus_with_end(int i) {
        if (i > 0) {
            while (i-- > 0) {
                if (m_focused_item == this->end())
                    m_focused_item = this->begin();
                else
                    m_focused_item++;
            }
        } else if (i < 0) {
            while (i++ < 0) {
                if (m_focused_item == this->begin())
                    m_focused_item = this->end();
                else
                    m_focused_item--;
            }
        }
    }

    void set_focus(list_iter iter) {
        //NOTE: this should check if iter is appropriate
        this->m_focused_item = iter;
    }

};

}  // namespace util
