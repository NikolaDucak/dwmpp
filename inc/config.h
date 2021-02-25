#pragma once 

#include "action/action_base.h"
#include "wm/client.h"
#include "wm/layout.h"
#include "wm/workspace.h"
#include "wm/bar.h"
#include "wm/window_manager.h"
#include "action/actions.h"
#include "util/restrict_to_range.h"

//TODO: figure out what to do with initializer list with unique ptr errror
//#define ACTION(act, arg)  std::make_unique<action:: act>(arg)
#define ACTION(act, arg)  new action:: act{ arg }

// clang-format off
const wm::client::config wm::client::conf {
    .border_width     = 3,
    .focused_border   { "#4ab" },
    .unfocused_border { "#333333" },
};

const wm::monitor::config wm::monitor::conf {
    .workspaces { "1","2","3","4","5","6","7","8","9" },
};

const wm::workspace::config wm::workspace::default_config {
    .default_layout_function = wm::tiling_layout,
    .layout {
        .enable_inner_gap = true,
        .enable_outer_gap = true,
        .inner_gap = 10,
        .outer_gap = 20,
        .master_percentage = 60,
        .number_of_masters = 1,
    },
};

const wm::bar::config wm::bar::conf {
    .fonts            { {"Iosevka Term"}, {"FontAwesome"} },
    .barBG            { "#30333d" },
    .barFG            { "#898a8f" },
    .empty_tag_bg     { "#585e74" },
    .empty_tag_fg     { "#4d4d4d" },
    .focused_tag_bg   { "#71c2ff" },
    .focused_tag_fg   { "#dd9999" },
    .unfocused_tag_bg { "#9ce5c0" },
    .unfocused_tag_fg { "#afafaf" },
};


auto increase_master = [] (wm::layout_config& cfg) {
    cfg.master_percentage = util::restrict_to_range(cfg.master_percentage+10, 10u, 90u);
};

auto decrease_master = [] (wm::layout_config& cfg) {
    cfg.master_percentage = util::restrict_to_range(cfg.master_percentage-10, 10u, 90u);
};

auto toggle_gaps = [](wm::layout_config& cfg) {
    cfg.enable_inner_gap = !cfg.enable_inner_gap;
    cfg.enable_outer_gap = !cfg.enable_outer_gap;
};


// Mod4Mask = "win"
// Mod1Mask = "alt"
static constexpr auto ModKey = Mod4Mask;
const wm::window_manager::config wm::window_manager::conf {
    .keybindings {

        // ending session
        keybinding { ModKey, XK_BackSpace, ACTION(quit_dwmpp,) },

        // closing a window
        keybinding { ModKey, XK_q,      ACTION(kill_focused,) },

        // floating currently focused window
        keybinding { ModKey, XK_f,      ACTION(toggle_floating,) },

        // setting layout for current workspace
        keybinding { ModKey | ShiftMask, XK_f,      ACTION(set_layout, fullscreen_layout) },
        keybinding { ModKey | ShiftMask, XK_t,      ACTION(set_layout, tiling_layout) },

        // moving focus
        keybinding { ModKey, XK_j,      ACTION(move_focus, +1) },
        keybinding { ModKey, XK_k,      ACTION(move_focus, -1) },

        // moving client on his stack
        keybinding { ModKey | ShiftMask, XK_j,      ACTION(move_focused, +1) },
        keybinding { ModKey | ShiftMask, XK_k,      ACTION(move_focused, -1) },

        // moving client to the top of the stack
        keybinding { ModKey | ShiftMask, XK_Return, ACTION(move_focused_to_top, ) },

        // spawning
        keybinding { ModKey, XK_D,      ACTION(spawn, "dmenu_run") },
        keybinding { ModKey, XK_Return, ACTION(spawn, "st") },

        // toggling bars visibility
        keybinding { ModKey, XK_b,      ACTION(toggle_bar, ) },

        // resising master area
        keybinding { ModKey, XK_h,      ACTION(layout_update<layout_config>, increase_master) },
        keybinding { ModKey, XK_l,      ACTION(layout_update<layout_config>, decrease_master) },

        // toggling gaps
        keybinding { ModKey, XK_g,      ACTION(layout_update<layout_config>, toggle_gaps) },

        // focus workspace
        keybinding { ModKey, XK_1,      ACTION(focus_workspace, 1) },
        keybinding { ModKey, XK_2,      ACTION(focus_workspace, 2) },
        keybinding { ModKey, XK_3,      ACTION(focus_workspace, 3) },
        keybinding { ModKey, XK_4,      ACTION(focus_workspace, 4) },
        keybinding { ModKey, XK_5,      ACTION(focus_workspace, 5) },
        keybinding { ModKey, XK_6,      ACTION(focus_workspace, 6) },
        keybinding { ModKey, XK_7,      ACTION(focus_workspace, 7) },
        keybinding { ModKey, XK_8,      ACTION(focus_workspace, 8) },
        keybinding { ModKey, XK_9,      ACTION(focus_workspace, 9) },

        // move focused client to wokrpsace
        keybinding { ModKey | ShiftMask, XK_1,      ACTION(move_focused_to_workspace, 1) },
        keybinding { ModKey | ShiftMask, XK_2,      ACTION(move_focused_to_workspace, 2) },
        keybinding { ModKey | ShiftMask, XK_3,      ACTION(move_focused_to_workspace, 3) },
        keybinding { ModKey | ShiftMask, XK_4,      ACTION(move_focused_to_workspace, 4) },
        keybinding { ModKey | ShiftMask, XK_5,      ACTION(move_focused_to_workspace, 5) },
        keybinding { ModKey | ShiftMask, XK_6,      ACTION(move_focused_to_workspace, 6) },
        keybinding { ModKey | ShiftMask, XK_7,      ACTION(move_focused_to_workspace, 7) },
        keybinding { ModKey | ShiftMask, XK_8,      ACTION(move_focused_to_workspace, 8) },
        keybinding { ModKey | ShiftMask, XK_9,      ACTION(move_focused_to_workspace, 9) },
    }
};

// clang-format on
