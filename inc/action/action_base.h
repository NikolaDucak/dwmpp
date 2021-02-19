#pragma once

#include "action/action_handler.h"

namespace action {

/**
 * Abstract base class representing action triggered on a keybinding specified in
 * the `config.h` meant to be executed by @ref action::action_handler in visitor pattern.
 */
struct action_base {
    virtual ~action_base() = default;
    virtual void execute(action_handler& handler) = 0;
};

}  // namespace action
