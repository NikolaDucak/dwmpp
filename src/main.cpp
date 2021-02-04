#include "xlib/XCore.h"
#include "config.h"
#include "xlib/XError.h"

int main() {
    xlib::err::check_for_other_wm();
    wm::window_manager wm { &xlib::XCore::instance() };
    wm.run();
}
