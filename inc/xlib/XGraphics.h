#pragma once

#include "XColor.h"
#include "XCore.h"
#include "XFont.h"
#include "XWindow.h"
#include "util/point.h"

#include <string>
#include <vector>

namespace xlib {

class XGraphics {
public:
    XGraphics();

    void fillRectangle(const XColor& c, point xy, point wh);

    void copyArea(Window w, point xy, point wh);
    int drawText(point xy, point wh, const std::string& textstr, std::vector<XFont>& fonts, const XColor& clr);
    int getTextWidth(const std::string& text, std::vector<XFont>& fonts);

    static XCore* xcore;

private:
    Pixmap drawable_;
    GC     gc_;
};

}  // namespace xlib
