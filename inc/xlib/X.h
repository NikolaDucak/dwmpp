#pragma once

#include "xlib/Xlib.h"
#include "xlib/XColor.h"
#include "xlib/XFont.h"
#include "xlib/XGraphics.h"
#include "xlib/XWindow.h"

namespace xlib {

    XCore& initializeXlib() { 
        static XCore x{0}; 
        XWindow::xcore = &x;
        XGraphics::xcore = &x;
        xlib::XFont::xcore = &x;
        xlib::XColor::xcore = &x;
        return x;
    }
}
