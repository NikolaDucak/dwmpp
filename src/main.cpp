#include <string>
#include <unistd.h>
#include <algorithm>

#define DEBUG_LOG


#include "xlib/X.h"
#include "wm/WindowManager.h"

// xlibinitalization firs
static xlib::XCore& x = xlib::initializeXlib();
static WindowManager* wm;

const Client::Config Client::config {
	.borderWidth       = 2,
	.borderClr         {"#2e2e2e"},
	.selecetedBorderClr{"#ff0000"},
};

// after xlib has been initialized, configuration can be set
Bar::Config Bar::config {
    .barBG        {"#2e2e2e"},
	.barFG        {"#999999"},

	.selectedBarBG{"#dddddd"},
	.selectedBarFG{"#dd9999"},

	.usedTagBG    {"#5f5f5f"},
	.usedTagFG    {"#afafaf"},

	.emptyTagBG   {"#3d3d3d"},
	.emptyTagFG   {"#4d4d4d"},

	.font         {"Iosevka"},
};

int main() {
    LOG("START");

    //TODO: xlib atoms
    //x.changeProperity() // change root name
    
    //TODO: error handlers
    // check for other window manager
    // err::otherWMCheck();
    
    // create WM instance
    static WindowManager wm{x};
    ::wm = &wm;
    
    // infine loop, run insntace
    wm.run();
    return 0;
}

//TODO: move to separate file
namespace config {

void moveClient(const Argument& arg){
    LOG("Config func triggered: moveClient");
    //wm->moveClient(arg.i);
}

void moveClientToWorkspace(const Argument& arg){
    LOG("Config func triggered: moveClientToWorkspace");
	//wm->moveClientToWorkspace(arg.i);
}

void focus(const Argument& arg){
    LOG("Config func triggered: focus");
	wm->moveFocus(arg.i);
}

void pkill(const Argument& /*unused*/){
    LOG("Config func triggered: pkill");
	wm->killFocused();
}

void quitDWMPP(const Argument& /*unused*/){
    LOG("Config func triggered: Quit");
	wm->quit();
}

void fullscreen(const Argument& /*unused*/){
    LOG("fullscreen");
	//wm->fullscreenToggle();
}

void floatToggle(const Argument& /*unused*/){
    LOG("floatToggle");
	//wm->floatToggle();
}

void goToWorkspace(const Argument& arg){
    LOG("Config func triggered: gotoWS");
	wm->goToWorkspace(arg.i);
}

void toggleBar(const Argument& /*unused*/){
    LOG("toggleBar");
	//wm->toggleBar();
}

void resizeMaster(const Argument& arg){
    LOG("resizeMaster");
	//wm->resizeMaster(arg.i);
}

void moveFloating(const Argument& /*unused*/){
    LOG("moveFloating");
	//wm->moveFloating();
}

void resizeFloating(const Argument& /*unused*/){
    LOG("resizeFloating");
	//wm->resizeFloating();
}

void moveToTop(const Argument& /*unused*/){
    LOG("floatToTop");
	//wm->moveFocusedToTop();
}

void spawn(const Argument& arg){
    LOG("Config func triggered: spawn");
    if (fork() == 0) {
        static char* args[] { NULL };
        setsid();
        execvp(arg.str, args);
        fprintf(stderr, "dwm: execvp %s", arg.str);
        perror("failed");
        exit(EXIT_SUCCESS);
    }
}
} // namespace config
