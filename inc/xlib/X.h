#pragma once

#include "xlib/Xlib.h"
#include "xlib/XColor.h"
#include "xlib/XFont.h"
#include "xlib/XGraphics.h"
#include "xlib/XWindow.h"

namespace xlib {

    /*
    inline XCore& initializeXlib() { 
        static XCore x{0}; 
        static bool initialized = false;
        if (not initialized) {
            XWindow::xcore      = &x;
            XGraphics::xcore    = &x;
            xlib::XFont::xcore  = &x;
            xlib::XColor::xcore = &x;
        }
        return x;
    }
    */
}
