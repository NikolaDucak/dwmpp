#include "xlib/XCore.h"
#include "config.h"
#include "xlib/XError.h"

int main() {
    // TODO: window manager should update netclientlist atom 
    xlib::err::check_for_other_wm();
    wm::window_manager wm { &xlib::XCore::instance() };
    wm.run();

    return 0;
}
