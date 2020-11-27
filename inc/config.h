#pragma once 

#include "action/action_base.h"
#include "wm/client.h"
#include "wm/layout.h"
#include "wm/workspace.h"
#include "wm/bar.h"
#include "wm/window_manager.h"
#include "action/actions.h"


//TODO: figure out what to do with initializer list with unique ptr errror
//#define ACTION(act, arg)  std::make_unique<action:: act>(arg)
#define ACTION(act, arg)  new action:: act{arg}
#define CONFIG_ACTION(act, func) new action:: act{ []( layout_config& conf ) func }

// clang-format off
const wm::client::config wm::client::conf {
    .border_width     = 3,
    .focused_border   { "#ff0000" },
    .unfocused_border { "#333333" },
};

const wm::workspace::config wm::workspace::conf {
    .workspaces { "1","2","3" },
    .layout {
        .inner_gap = 10,
        .outer_gap = 20,
        .master_percentage = 60,
        .number_of_masters = 1,
    },
};

const wm::bar::config wm::bar::conf {
    .font             { "Iosevka" },
    .barBG            { "#777777" },
    .barFG            { "#aaaaaa" },
    .empty_tag_bg     { "#3d3d3d" }, 
    .empty_tag_fg     { "#4d4d4d" },
    .focused_tag_bg   { "#dddddd" }, 
    .focused_tag_fg   { "#dd9999" },
    .unfocused_tag_bg { "#5f5f5f" }, 
    .unfocused_tag_fg { "#afafaf" },
};

static constexpr auto ModKey = Mod4Mask;

const wm::window_manager::config wm::window_manager::conf {
    .keybindings {
        //keybinding { ModKey, XK_l,      CONFIG_ACTION(layout_config, {conf.inner_gap+=10;}) },

        // ending session
        keybinding { ModKey, XK_BackSpace, ACTION(quit_dwmpp,) },

        // closing a window
        keybinding { ModKey, XK_q,      ACTION(kill_focused,) },

        // floating toggle
        keybinding { ModKey, XK_f,      ACTION(toggle_floating,) },

        // setting layout
        keybinding { ModKey | ShiftMask, XK_f,      ACTION(set_layout, fullscreen_layout) },
        keybinding { ModKey | ShiftMask, XK_t,      ACTION(set_layout, tiling_layout) },

        // moving focus
        keybinding { ModKey, XK_j,      ACTION(move_focus, +1) },
        keybinding { ModKey, XK_k,      ACTION(move_focus, -1) },

        // spawning
        keybinding { ModKey, XK_D,      ACTION(spawn, "dmenu_run") },
        keybinding { ModKey, XK_Return, ACTION(spawn, "st") },

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
        keybinding { ModKey, XK_9,      ACTION(focus_workspace, 0) },

        // move focused client to wokrpsace
        keybinding { ModKey | ShiftMask, XK_1,      ACTION(move_focused_to_workspace, 1) },
        keybinding { ModKey | ShiftMask, XK_2,      ACTION(move_focused_to_workspace, 2) },
        keybinding { ModKey | ShiftMask, XK_3,      ACTION(move_focused_to_workspace, 3) },
        keybinding { ModKey | ShiftMask, XK_4,      ACTION(move_focused_to_workspace, 4) },
        keybinding { ModKey | ShiftMask, XK_5,      ACTION(move_focused_to_workspace, 5) },
        keybinding { ModKey | ShiftMask, XK_6,      ACTION(move_focused_to_workspace, 6) },
        keybinding { ModKey | ShiftMask, XK_7,      ACTION(move_focused_to_workspace, 7) },
        keybinding { ModKey | ShiftMask, XK_8,      ACTION(move_focused_to_workspace, 8) },
        keybinding { ModKey | ShiftMask, XK_9,      ACTION(move_focused_to_workspace, 0) },
    }
};

// clang-format on
