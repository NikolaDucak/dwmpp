#pragma once

#include "XCore.h"

#include <X11/Xft/Xft.h>
#include <string>

namespace xlib {

class XFont {
public:
    // TODO: should be marked as explicit but breaks config.h since it relies
    // on automatic construction of strings to xfont for readability.
    XFont(const std::string& fontname);
    explicit XFont(FcPattern* fontpattern);

    auto get() { return xfont_; }
    const auto get() const { return xfont_; }
    auto getPattern() { return pattern_; }
    int getHeight() const { return height_; }

    unsigned getTextExtents(const char* text, int len) const;
    int getTextWidthInPixels(const std::string& text) const;

    static XCore* xcore;
private:
    XftFont* xfont_;
    FcPattern* pattern_;
    int height_;
    std::string nm;
};



} // namespace xlib
