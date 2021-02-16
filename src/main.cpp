#include "xlib/XCore.h"
#include "config.h"
#include "xlib/XError.h"

/* TODO: 
 * - window manager should update netclientlist atom 
 * - supporting window for NetWMCheck 
 * - cleanup for XColor, XFont... unmanage all clients
 */

int main() {
    xlib::err::check_for_other_wm();
    wm::window_manager wm { &xlib::XCore::instance() };
    wm.run();
    return 0;
}
