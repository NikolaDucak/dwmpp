#pragma once

#include <string>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "ConfigTypes.h"


namespace config {


static const auto& LeftClick = Button1;
//static const auto& MiddletClick = Button3;
static const auto& RightClick = Button3;




/*
 * Config values
 */

static constexpr int UP   = -1;
static constexpr int DOWN = +1;
static constexpr auto ModKey = Mod4Mask;
static const WMConfig windowManager { 
	.keybindings {
		// end  session
		KeyBinding{   ModKey,             XK_BackSpace,   quitDWMPP,     {}              },

		// resize master
		KeyBinding{   ModKey,     		XK_l,           resizeMaster,		{.i = DOWN}     },
		KeyBinding{   ModKey,     		XK_h,           resizeMaster,    	{.i = UP}       },

		// basic run command
		KeyBinding{   ModKey,           XK_Return,      spawn,    {.str = "st"}       },
		KeyBinding{   ModKey,           XK_d,           spawn,    {.str = "dmenu_run"}},

		// move to top of client stack
		KeyBinding{   ModKey|ShiftMask, XK_Return,      moveToTop,        {}       },

		// toggle bar
		KeyBinding{   ModKey,     		XK_b,           toggleBar,    	{}       },

		// kill focused
		{   ModKey,     		XK_q,           pkill,                    {}       },

		// modify client
		{   ModKey,     		XK_f,           fullscreen,               {}       },
		{   ModKey|ShiftMask,	XK_f,           floatToggle,   			{}       },
		
		// movig focus
		{   ModKey,     		XK_j,           focus,    			{.i = DOWN}     },
		{   ModKey,     		XK_k,           focus,    			{.i = UP}       },

		// moving client on stack
		{   ModKey|ShiftMask,	XK_j,           moveClient,			{.i = DOWN}     },
		{   ModKey|ShiftMask,	XK_k,           moveClient,			{.i = UP}       },


		// moving client to other workspaces
		{   ModKey|ShiftMask,	XK_1,           moveClientToWorkspace,	{.i = 0}       },
		{   ModKey|ShiftMask,	XK_2,           moveClientToWorkspace,	{.i = 1}       },
		{   ModKey|ShiftMask,	XK_3,           moveClientToWorkspace,	{.i = 2}       },
		{   ModKey|ShiftMask,	XK_4,           moveClientToWorkspace,	{.i = 3}       },
		{   ModKey|ShiftMask,	XK_5,           moveClientToWorkspace,	{.i = 4}       },
		{   ModKey|ShiftMask,	XK_6,           moveClientToWorkspace,	{.i = 5}       },
		{   ModKey|ShiftMask,	XK_7,           moveClientToWorkspace,	{.i = 6}       },
		{   ModKey|ShiftMask,	XK_8,           moveClientToWorkspace,	{.i = 7}       },
		{   ModKey|ShiftMask,	XK_9,           moveClientToWorkspace,	{.i = 8}       },
		{   ModKey|ShiftMask,	XK_9,           moveClientToWorkspace,	{.i = 9}       },

		// view workspace
		{   ModKey,     		XK_1,           goToWorkspace,    {.i = 0}       },
		{   ModKey,     		XK_2,           goToWorkspace,    {.i = 1}       },
		{   ModKey,     		XK_3,           goToWorkspace,    {.i = 2}       },
		{   ModKey,     		XK_4,           goToWorkspace,    {.i = 3}       },
		{   ModKey,     		XK_5,           goToWorkspace,    {.i = 4}       },
		{   ModKey,     		XK_6,           goToWorkspace,    {.i = 5}       },
		{   ModKey,     		XK_7,           goToWorkspace,    {.i = 6}       },
		{   ModKey,     		XK_8,           goToWorkspace,    {.i = 7}       },
		{   ModKey,     		XK_9,           goToWorkspace,    {.i = 8}       },
		{   ModKey,     		XK_9,           goToWorkspace,    {.i = 9}       },
	},

	.mouseBindings{
		MouseBinding{ ClickLocation::Client, ModKey, LeftClick,  moveFloating, {}  },
		MouseBinding{ ClickLocation::Client, ModKey, RightClick, resizeFloating, {}  },
	},
};
static const BarConfig bar{ };
static const ClientConfig client{ };
static const WorkspaceConfig workspace { };

} // namespace config
