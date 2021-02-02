#include "xlib/XCore.h"
#include "config.h"

namespace wm{
namespace err {

    void check_for_other_wm();
    void dummy();
}
}

int main() {
    wm::window_manager wm { &xlib::XCore::instance() };
    wm.run();
}
