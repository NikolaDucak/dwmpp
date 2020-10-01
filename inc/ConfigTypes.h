#pragma once

#include <X11/Xlib.h>

#include <functional>

static const auto& LeftClick = Button1;
static const auto& MiddletClick = Button3;
static const auto& RightClick = Button3;

union Argument { int i; const char* str; };

struct KeyBinding {
	int mod;
	KeySym keySym;
	std::function< void(const Argument&) > action;
	Argument a;
};

enum class ClickLocation{
	TagBar, StatusBar, TitleBar, Client
};

struct MouseBinding{
	ClickLocation click;
	unsigned int mask;
	unsigned int button;
	std::function<void(const Argument&)> action;
	const Argument arg;
};

// key actions
void quitDWMPP     (const Argument& /*unused*/);
void fullscreen    (const Argument& /*unused*/);
void floatToggle   (const Argument& /*unused*/);
void pkill         (const Argument& /*unused*/);
void toggleBar     (const Argument& /*unused*/);
void moveToTop     (const Argument& /*unused*/);
void moveFloating  (const Argument& /*unused*/);
void resizeFloating(const Argument& /*unused*/);
void shell         (const Argument& string);
void spawn         (const Argument& string);
void focus         (const Argument& direction);
void moveClient    (const Argument& direction);
void goToWorkspace (const Argument& num);
void resizeMaster  (const Argument& num);
void moveClientToWorkspace(const Argument& num);
