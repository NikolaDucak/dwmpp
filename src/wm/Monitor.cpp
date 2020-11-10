#include "wm/Monitor.h"

void Monitor::updateMonitors( std::list<Monitor>& currentMonitors ) {
	// TODO: implementaiton
 	currentMonitors.emplace_back( 1, point{0,0}, point{1200,700}, point{0,0}, point{1200,700} );
}

Monitor::Monitor(int num, point xy, point wh, point win_xy, point win_wh) :
    num(num), xy_(xy), wh_(wh), win_xy_(win_xy),
    win_wh_(win_wh), workspaces_ { { { *this, 0 },
                                     { *this, 1 },
                                     { *this, 2 },
                                     { *this, 3 },
                                     { *this, 4 },
                                     { *this, 5 },
                                     { *this, 6 },
                                     { *this, 7 },
                                     { *this, 8 },
                                     { *this, 9 } } },
    selectedWorkspaceIndex_(0)
//	bar_( wh.x )
{}

void Monitor::selectWorkspace(unsigned i) {
    if (i < Workspace::config.workspaces.size()) {
        getSelectedWorkspace().hideAllClients();
        selectedWorkspaceIndex_ = i;
        getSelectedWorkspace().showAllClients();
    }
}
