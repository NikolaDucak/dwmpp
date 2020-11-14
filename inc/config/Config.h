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

const Bar::Config Bar::config {
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

static constexpr auto ModKey = Mod4Mask;

const WindowManager::Config WindowManager::config {
    .keybindings {
        // -------------------------------------------------------------------------------
        //         |       MODIFIERS     |     KEY       |  ACTION, ARGUMENT 
        // -------------------------------------------------------------------------------
        // end  session
        KeyBinding { ModKey,               XK_BackSpace,   quitDWMPP, {} },

        // resize master
        KeyBinding { ModKey,               XK_l,           config::resizeMaster, { DOWN } },
        KeyBinding { ModKey,               XK_h,           config::resizeMaster, { UP } },

        // basic run command
        KeyBinding { ModKey,               XK_Return,      spawn,{ .str= "st" } },
        KeyBinding { ModKey,               XK_d,           spawn,{ .str= "dmenu_run" } },

        // move to top of client stack
        KeyBinding { ModKey | ShiftMask,   XK_Return,      moveToTop, {} },

        // toggle bar
        KeyBinding { ModKey,               XK_b,           config::toggleBar, {} },

        // kill focused
        KeyBinding { ModKey,               XK_q,           pkill, {} },

        // modify client
        KeyBinding { ModKey,               XK_f,           fullscreen, {} },
        KeyBinding { ModKey | ShiftMask,   XK_f,           ::floatToggle, {} },

        // movig focus
        KeyBinding { ModKey,               XK_j,           focus, { DOWN } },
        KeyBinding { ModKey,               XK_k,           focus, { UP } },

        // moving client on stack
        KeyBinding { ModKey | ShiftMask,   XK_j,           moveClient, { DOWN } },
        KeyBinding { ModKey | ShiftMask,   XK_k,           moveClient, { UP } },

        // moving client to other workspaces
        KeyBinding { ModKey | ShiftMask,   XK_1,           moveClientToWorkspace, { 0 } },
        KeyBinding { ModKey | ShiftMask,   XK_2,           moveClientToWorkspace, { 1 } },
        KeyBinding { ModKey | ShiftMask,   XK_3,           moveClientToWorkspace, { 2 } },
        KeyBinding { ModKey | ShiftMask,   XK_4,           moveClientToWorkspace, { 3 } },
        KeyBinding { ModKey | ShiftMask,   XK_5,           moveClientToWorkspace, { 4 } },
        KeyBinding { ModKey | ShiftMask,   XK_6,           moveClientToWorkspace, { 5 } },
        KeyBinding { ModKey | ShiftMask,   XK_7,           moveClientToWorkspace, { 6 } },
        KeyBinding { ModKey | ShiftMask,   XK_8,           moveClientToWorkspace, { 7 } },
        KeyBinding { ModKey | ShiftMask,   XK_9,           moveClientToWorkspace, { 8 } },
        KeyBinding { ModKey | ShiftMask,   XK_9,           moveClientToWorkspace, { 9 } },

        // view workspace
        KeyBinding { ModKey, XK_1, ::goToWorkspace, { 0 } },
        KeyBinding { ModKey, XK_2, ::goToWorkspace, { 1 } },
        KeyBinding { ModKey, XK_3, ::goToWorkspace, { 2 } },
        KeyBinding { ModKey, XK_4, ::goToWorkspace, { 3 } },
        KeyBinding { ModKey, XK_5, ::goToWorkspace, { 4 } },
        KeyBinding { ModKey, XK_6, ::goToWorkspace, { 5 } },
        KeyBinding { ModKey, XK_7, ::goToWorkspace, { 6 } },
        KeyBinding { ModKey, XK_8, ::goToWorkspace, { 7 } },
        KeyBinding { ModKey, XK_9, ::goToWorkspace, { 8 } },
        KeyBinding { ModKey, XK_9, ::goToWorkspace, { 9 } },
    },

    .mouseBindings {
        MouseBinding { ClickLocation::Client, ModKey, LeftClick,    ::moveFloating,   {} },
        MouseBinding { ClickLocation::Client, ModKey, RightClick,   ::resizeFloating, {} },
        MouseBinding { ClickLocation::Client, ModKey, MiddletClick, ::resizeFloating, {} },
    },
};

