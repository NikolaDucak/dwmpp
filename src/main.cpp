#include "xlib/XCore.h"
#include "config.h"
#include "xlib/XError.h"

int main() {
    //TODO: maybe more elegant solution
    //this should be called first to initialize
    //original error handler
    xlib::err::check_for_other_wm();

    wm::window_manager wm { &xlib::XCore::instance() };

    wm.run();
}
