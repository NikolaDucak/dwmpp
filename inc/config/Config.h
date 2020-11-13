#pragma once

#include <string>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "ConfigTypes.h"

#include "wm/Bar.h"
#include "wm/Client.h"
#include "wm/Workspace.h"
#include "wm/WindowManager.h"

using namespace config;

const Client::Config Client::config {
	.borderWidth       = 2,
	.borderClr         {"#2e2e2e"},
	.selecetedBorderClr{"#ff0000"},
};

// after xlib has been initialized, configuration can be set
Bar::Config Bar::config {
    .barBG        {"#2e2e2e"},
	.barFG        {"#999999"},

	.selectedTagBG{"#dddddd"},
	.selectedTagFG{"#dd9999"},

	.usedTagBG    {"#5f5f5f"},
	.usedTagFG    {"#afafaf"},

	.emptyTagBG   {"#3d3d3d"},
	.emptyTagFG   {"#4d4d4d"},

	.font         {"Iosevka"},
};

const Workspace::Config Workspace::config {
    .workspaces = { "1", "2", "3", "4", "5", "6", "7", "8", "9" "+" },
    .innerGap   = 10,
    .outerGap   = 10,
    .factor     = 60,
    .mastersNum = 1,
};

const  WindowManager::Config WindowManager::config {
    .keybindings {
        // end  session
        KeyBinding { ModKey,             XK_BackSpace, quitDWMPP, {} },

        // resize master
        KeyBinding { ModKey,             XK_l,      config::resizeMaster, { .i = DOWN } },
        KeyBinding { ModKey,             XK_h,      config::resizeMaster, { .i = UP } },

        // basic run command
        KeyBinding { ModKey,             XK_Return, spawn,     { .str = "st" } },
        KeyBinding { ModKey,             XK_d,      spawn,     { .str = "dmenu_run" } },

        // move to top of client stack
        KeyBinding { ModKey | ShiftMask, XK_Return, moveToTop, {} },

        // toggle bar
        KeyBinding { ModKey, XK_b, config::toggleBar, {} },

        // kill focused
        { ModKey, XK_q, pkill, {} },

        // modify client
        { ModKey, XK_f, fullscreen, {} },
        { ModKey | ShiftMask, XK_f, config::floatToggle, {} },

        // movig focus
        { ModKey, XK_j, focus, { .i = DOWN } },
        { ModKey, XK_k, focus, { .i = UP } },

        // moving client on stack
        { ModKey | ShiftMask, XK_j, moveClient, { .i = DOWN } },
        { ModKey | ShiftMask, XK_k, moveClient, { .i = UP } },

        // moving client to other workspaces
        { ModKey | ShiftMask, XK_1, moveClientToWorkspace, { .i = 0 } },
        { ModKey | ShiftMask, XK_2, moveClientToWorkspace, { .i = 1 } },
        { ModKey | ShiftMask, XK_3, moveClientToWorkspace, { .i = 2 } },
        { ModKey | ShiftMask, XK_4, moveClientToWorkspace, { .i = 3 } },
        { ModKey | ShiftMask, XK_5, moveClientToWorkspace, { .i = 4 } },
        { ModKey | ShiftMask, XK_6, moveClientToWorkspace, { .i = 5 } },
        { ModKey | ShiftMask, XK_7, moveClientToWorkspace, { .i = 6 } },
        { ModKey | ShiftMask, XK_8, moveClientToWorkspace, { .i = 7 } },
        { ModKey | ShiftMask, XK_9, moveClientToWorkspace, { .i = 8 } },
        { ModKey | ShiftMask, XK_9, moveClientToWorkspace, { .i = 9 } },

        // view workspace
        { ModKey, XK_1, config::goToWorkspace, { .i = 0 } },
        { ModKey, XK_2, config::goToWorkspace, { .i = 1 } },
        { ModKey, XK_3, config::goToWorkspace, { .i = 2 } },
        { ModKey, XK_4, config::goToWorkspace, { .i = 3 } },
        { ModKey, XK_5, config::goToWorkspace, { .i = 4 } },
        { ModKey, XK_6, config::goToWorkspace, { .i = 5 } },
        { ModKey, XK_7, config::goToWorkspace, { .i = 6 } },
        { ModKey, XK_8, config::goToWorkspace, { .i = 7 } },
        { ModKey, XK_9, config::goToWorkspace, { .i = 8 } },
        { ModKey, XK_9, config::goToWorkspace, { .i = 9 } },
    },

    .mouseBindings {
        MouseBinding {
            ClickLocation::Client, ModKey, LeftClick, config::moveFloating, {} },
        MouseBinding {
            ClickLocation::Client, ModKey, RightClick, config::resizeFloating, {} },
        MouseBinding {
            ClickLocation::Client, ModKey, MiddletClick, config::resizeFloating, {} },
    },
};

