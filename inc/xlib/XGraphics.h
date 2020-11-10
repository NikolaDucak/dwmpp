#pragma once

#include "Xlib.h"
#include "XWindow.h"
#include "XColor.h"
#include "XFont.h"
#include "util/point.h"
#include <string>
#include <vector>

namespace xlib {

class XGraphics {

public:
    XGraphics();

    void drawText(XWindow& w, const XFont& fnt, const XColor& clr, 
                  point xy, const std::string& text);

    void fillRectangle(const XColor& c, point xy, point wh);

    void copyArea(Window w, point xy, point wh);

    static XCore* xcore;

private:

    Pixmap drawable_;
    GC gc_;
};


} // namespace xlib
