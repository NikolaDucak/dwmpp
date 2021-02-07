#include "xlib/XCore.h"
#include "config.h"
#include "xlib/XError.h"

/* TODO: 
 * - window manager should update netclientlist atom 
 * - switching layout *sometimes* crashex dwmpp
 * - FIX: toggling `floating` on client crashes dwmpp
 *      retproduce: win+return x3
 *      win + k x2 to select middle client
 *      win + f 
 */

int main() {
    xlib::err::check_for_other_wm();
    wm::window_manager wm { &xlib::XCore::instance() };
    wm.run();

    return 0;
}
