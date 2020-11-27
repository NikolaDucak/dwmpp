#pragma once

#include "action/action_handler.h"

namespace action {

struct action_base {
    virtual ~action_base() = default;
    virtual void execute(action_handler& handler) = 0;
};

}  // namespace action
