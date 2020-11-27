#include "xlib/XCore.h"
#include "config.h"

int main() {
    wm::window_manager wm { &xlib::XCore::instance() };
    wm.run();
}
