#pragma once

#include "xlib/Xlib.h"

#include <X11/Xft/Xft.h>
#include <string>

namespace xlib {

class XColor {
public:
    static XCore* xcore;
    XColor(const std::string& colorName);

    inline const XftColor& get() const { return m_xftColor; }
private:

    XftColor m_xftColor;
};

} // namespace xlib
