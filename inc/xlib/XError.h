#pragma once

#include <X11/Xlib.h>
#include "xlib/XCore.h"

namespace xlib {
namespace err {

using handler_t = int (*)(Display*, XErrorEvent*);

// save default handler
void init();

void check_for_other_wm();

void use_dummy_err_handler();

void use_original_err_handler();

}  // namespace err
}  // namespace xlib
