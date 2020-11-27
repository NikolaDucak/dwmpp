#include "xlib/XColor.h"

namespace xlib {

namespace util {
    void die( const std::string& s ){
        printf("%s", s.c_str());
        exit(0);
    }
}

XCore* XColor::xcore = &XCore::instance();

XColor::XColor(const std::string& clrname) {

    auto dpy = XCore::instance().getDpyPtr();

    if (!XftColorAllocName(dpy, DefaultVisual(dpy, DefaultScreen(dpy)),
                           DefaultColormap(dpy, DefaultScreen(dpy)),
                           clrname.c_str(), &m_xftColor))
        util::die("error, cannot allocate color ");
}

} // namespace xlib
