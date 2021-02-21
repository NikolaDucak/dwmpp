#include "xlib/XCore.h"
#include "config.h"
#include "xlib/XError.h"

/* TODO: 
 * - window manager should update netclientlist atom 
 * - cleanup for XColor, XFont...
 * - focus is forced to stay on the mouse, so moving client on the stack
 *   triggers refocus to new client, making it hard to move client more than one place
 */

int main() {
    xlib::err::check_for_other_wm();
    wm::window_manager wm { &xlib::XCore::instance() };
    wm.run();
    return 0;
}
