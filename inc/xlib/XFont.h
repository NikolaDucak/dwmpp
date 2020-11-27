#pragma once

#include "XCore.h"

#include <X11/Xft/Xft.h>
#include <string>

namespace xlib {

class XFont {
public:
    XFont(FcPattern* fontpattern);
    XFont(const std::string& fontname);

    int getTextWidthInPixels(const std::string& text) const;
    XftFont* get() { return xfont_; }
    const XftFont* get() const { return xfont_; }
    int getHeight() const { return height_; }

    static XCore* xcore;

private:
    XftFont* xfont_;
    FcPattern* pattern_;
    int height_;
};



} // namespace xlib
