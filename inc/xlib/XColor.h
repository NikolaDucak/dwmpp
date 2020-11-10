#pragma once

#include "xlib/Xlib.h"
#include <string>
#include <X11/Xft/Xft.h>

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
