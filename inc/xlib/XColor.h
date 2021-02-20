#pragma once

#include "xlib/XCore.h"
#include "util/point.h"

#include <X11/Xft/Xft.h>
#include <string>

namespace xlib {

using util::point;

class XColor {
public:
    static XCore* xcore;
    explicit XColor(const std::string& colorName);

    inline const XftColor& get() const { return m_xftColor; }
private:

    XftColor m_xftColor;
};

} // namespace xlib
