#include "xlib/XError.h"
#include <X11/Xlib.h>

namespace xlib {
namespace err {

static handler_t original_handler;

int dummy(Display* /*unsued*/, XErrorEvent* /*unused*/) { return 0; }

void init() { 
    original_handler = XSetErrorHandler(dummy);
    XSetErrorHandler(original_handler);
}

void use_dummy_err_handler() { XSetErrorHandler(dummy); }

void use_original_err_handler() { XSetErrorHandler(original_handler); }

int start(Display* /*unsued*/, XErrorEvent* /*unused*/) {
    puts("dwmpp: another window manager is already running");
    exit(-1);
    return 0;
}

void check_for_other_wm() {
    auto dpy         = XCore::instance().getDpyPtr();
    original_handler = XSetErrorHandler(start);
    /* this causes an error if some other window manager is running */
    XSelectInput(dpy, DefaultRootWindow(dpy), SubstructureRedirectMask);
    XSync(dpy, False);
    XSetErrorHandler(original_handler);
    XSync(dpy, False);
}

}  // namespace err

}  // namespace xlib
