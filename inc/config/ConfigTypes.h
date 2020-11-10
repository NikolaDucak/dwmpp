#pragma once

#include <functional>
#include <vector>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

namespace config {

static const auto& LeftClick = Button1;
static const auto& MiddletClick = Button3;
static const auto& RightClick = Button3;


/*
 * Config values
 */

static constexpr int UP   = -1;
static constexpr int DOWN = +1;
static constexpr auto ModKey = Mod4Mask;

union Argument { int i; const char* str; };
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

/*
 *  Config types
 */
struct ClientConfig {
    unsigned borderWidth;
    std::string borderColor;
};


struct WMConfig { 
    std::vector<KeyBinding> keybindings;
    std::vector<MouseBinding> mouseBindings;
};

struct BarConfig { 
    std::string font;
    std::string bgcolor;

    std::string tagEmptyClr;
    std::string tagSelectedClr;
    std::string tagFullClr;
};

struct WorkspaceConfig { 
    std::vector<std::string> workspaces;
    int innerGap, outerGap;
    int factor, mastersNum;
};

}
