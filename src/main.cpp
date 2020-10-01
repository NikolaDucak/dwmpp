#include <unistd.h>

#include "WM.h"
#include "Config.h"

int main() 
{
	// set title string
	XLib::instance().changeProperty( 
			XLib::instance().getRoot(),
			NetWMName, UTF8String, 
			(unsigned char*)"dwmpp-0.01", 10 );

	// run wm
	WM& wm = WM::instance();
	wm.run();
}

/*===========================================================================================
 *			Key actions
 *===========================================================================================*/

void moveClient( const Argument& arg ){
	WM::instance().moveClient( arg.i );
}

void moveClientToWorkspace( const Argument& arg ){
	WM::instance().moveClientToWorkspace( arg.i );
}

void focus( const Argument& arg ){
	WM::instance().moveFocus( arg.i );
}

void pkill( const Argument& /*unused*/ ){
	WM::instance().killFocused();
}

void quitDWMPP( const Argument& /*unused*/ ){
	WM::instance().quit();
}

void fullscreen( const Argument& /*unused*/ ){
	WM::instance().fullscreenToggle();
}

void floatToggle( const Argument& /*unused*/ ){
	WM::instance().floatToggle();
}

void goToWorkspace( const Argument& arg ){
	WM::instance().goToWorkspace( arg.i );
}

void toggleBar( const Argument& /*unused*/ ){
	WM::instance().toggleBar();
}

void resizeMaster( const Argument& arg ){
	WM::instance().resizeMaster( arg.i );
}

void moveFloating( const Argument& /*unused*/ ){
	WM::instance().moveFloating();
}

void resizeFloating( const Argument& /*unused*/ ){
	WM::instance().resizeFloating();
}

void moveToTop( const Argument& /*unused*/ ){
	WM::instance().moveFocusedToTop();
}

void spawn( const Argument& arg ){
	if( fork() == 0 ){
		static char* args[]{ NULL };
		setsid();
		execvp( arg.str, args );
		fprintf(stderr, "dwm: execvp %s", arg.str);
		perror(" failed");
		exit(EXIT_SUCCESS);
	}
}
