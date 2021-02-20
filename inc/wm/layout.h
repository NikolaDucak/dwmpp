#pragma once

#include "util/area.h"
#include "util/focus_list.h"
#include "wm/client.h"

#include <functional>
#include <iostream>

namespace wm {

struct layout_config {
    unsigned inner_gap;
    unsigned outer_gap;
    unsigned master_percentage;
    unsigned number_of_masters;
};

using layout_function = std::function<void(
    util::focus_list<client>&, const layout_config& layout, const util::rect&)>;

/**
 * If @p clients contains only one clients, cliet takes all space else
 * clients takes master area and other clients split equaly slave area
 */
void tiling_layout(util::focus_list<client>& clients, const layout_config& cfg,
                   const util::rect& area);

/**
 * Resizes all clients over the whole @p area and raises focused client.
 */
void fullscreen_layout(util::focus_list<client>& clients,
                       const layout_config& cfg, const util::rect& area);


}  // namespace wm
