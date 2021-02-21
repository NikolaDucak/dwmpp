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

    void drawText(const XFont& fnt, const XColor& clr, point xy,
                  const std::string& text);

    void fillRectangle(const XColor& c, point xy, point wh);

    void copyArea(Window w, point xy, point wh);
    void dwmDrawtext(int x, int y, unsigned int w, unsigned int h,
             unsigned int lpad, const char* text, int invert);

    static XCore* xcore;

private:
    Pixmap drawable_;
    GC     gc_;
};

}  // namespace xlib
