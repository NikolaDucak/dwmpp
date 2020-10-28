#include <string>
#include <unistd.h>
#include <algorithm>

#define DEBUG_LOG

#include "config/Config.h"

#include "wm/Client.h"
#include "wm/WindowManager.h"
#include "wm/Bar.h"

WindowManager* wm;

int main() {
    LOG("START");
    using namespace xlib;
    // initialize xlib (open display etc)
    XCore x{0};
    XWindow::xcore = &x;

    //TODO: xlib atoms
    //x.changeProperity() // change root name
    
    //TODO: error handlers
    // check for other window manager
    // err::otherWMCheck();
    
    // copy config to internal config struct for
    // WM, Workspace, Client, Bar and do necessary prep and processing
    Bar::configure(config::bar);
    Client::configure(config::client);
    Workspace::configure(config::workspace);
    WindowManager::configure(config::windowManager);

    // create WM instance
    WindowManager wm{x};
    
    // run insntace
    wm.run();
    ::wm = &wm;

    return 0;
}

//TODO: move to separate file
namespace config {

void moveClient(const Argument& arg){
    LOG("moveClient");
    //wm->moveClient(arg.i);
}

void moveClientToWorkspace(const Argument& arg){
    LOG("moveClientToWorkspace");
	//wm->moveClientToWorkspace(arg.i);
}

void focus(const Argument& arg){
    LOG("focus");
	//wm->moveFocus(arg.i);
}

void pkill(const Argument& /*unused*/){
    LOG("pkill");
	//wm->killFocused();
}

void quitDWMPP(const Argument& /*unused*/){
    LOG("Quit");
	//wm->quit();
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
    LOG("gotoWS");
	//wm->goToWorkspace(arg.i);
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
    LOG("spawn");
	if(fork() == 0){
		static char* args[]{ NULL };
		setsid();
		execvp(arg.str, args);
		fprintf(stderr, "dwm: execvp %s", arg.str);
		perror(" failed");
		exit(EXIT_SUCCESS);
	}
}
} // namespace config
