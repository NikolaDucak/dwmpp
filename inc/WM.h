#pragma once

#include "Monitor.h"
#include <vector>
#include "ConfigTypes.h"
#include "XLibWrapper.h"

#ifdef DEBUG
#define LOG(x) std::cout << x << std::endl
#else
#define LOG(x)
#endif

class WM {
public:
	// Configuration
	struct Config{
		std::vector<KeyBinding> keybindings;
		std::vector<MouseBinding> mouseBindings;
	};
	static const Config config;

private:

	WM( XLib& xvars );

public:

	static WM& instance();

	void run();
	void quit();
	void moveFocus( int i );
	void moveClient( int i );
	void moveClientToWorkspace( uint i );
	void killFocused();
	void fullscreenToggle();
	void floatToggle();
	void goToWorkspace( int i );
	void resizeMaster ( int i );
	void resizeFloating ();
	void moveFloating();
	void toggleBar();
	void moveFocusedToTop();


private:
	// notifications
	void onMotionNotify     ( const XMotionEvent& e );
	void onProperityNotify  ( const XPropertyEvent& e );
	void onMappingNotify    ( XMappingEvent& e );
	void onUnmapNotify      ( const XUnmapEvent& e );
	void onConfigureNotify  ( const XConfigureEvent& e );
	void onDestroyNotify    ( const XDestroyWindowEvent& e );
	void onEnterNotify      ( const XCrossingEvent& e );
	// requests
	void onConfigureRequest ( const XConfigureRequestEvent& e);
	void onMapRequest       ( const XMapRequestEvent& e );
	// input
	void onKeyPress         ( const XKeyEvent& e );
	void onButtonPress      ( const XButtonPressedEvent& e );
	// events
	void onClientMessage    ( const XClientMessageEvent& e );
	void onExpose           ( const XExposeEvent& e );
	void onFocusIn          ( const XFocusChangeEvent& e );

	// util
	Monitor* getMonitorForRectangle( point xy, point wh );
	Monitor* getMonitorForWindow( Window w );
	Client*  getClientForWindow( Window w );
	void updateClientList();

	void focusClient   ( Client& c );
	void unfocusClient ( Client& c );
	void unmanageClient( Client& client );
	// initialization helpers
	void grabKeys();
	void grabButtons( Client& c );
	// helper
	void arrangeClients( Workspace& ws );

	// variables
	bool    running_;
	XLib&  x_;
	XAtom  xatoms_;
	std::list< Monitor > monitors_; 
	Monitor             *selectedMonitor_;
};
