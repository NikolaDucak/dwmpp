#pragma once

#include "xlib/Xlib.h" 
#include "config/Config.h"
#include "Monitor.h"
#include <unordered_map>

#define DEBUG_LOG

#ifdef DEBUG_LOG
#    include <iostream>
#    define LOG(x) std::cout << (x) << std::endl;
#else 
#    define LOG(x)
#endif

class WindowManager {
public:
    WindowManager(xlib::XCore& x);

    void run();

    /* methods called on key events, interface with the user */
    void quit();
    void moveFocus(int i);
    void moveFocusedClient(int i);
    void moveFocusedClientToTop();
    void moveFocusedClientToWorkspace(uint i);
    void killFocused();
    void fullscreenToggle();
    void floatToggle();
    void goToWorkspace(int i);
    void resizeMaster(int i);
    void resizeFloating();
    void moveFloating();
    void toggleBar();

private: // event functions 

    /** Method called when user mooves the crusor. */
	void onMotionNotify     (const XMotionEvent& e);       
    /** Actionbtrigeret when atom properity of window is changed. */
	void onProperityNotify  (const XPropertyEvent& e);
    /** Event trigered on changing keyboard mapping. */
	void onMappingNotify    (XMappingEvent& e);
    /** Event triggered when a client doesn't want to be drawn. */
	void onUnmapNotify      (const XUnmapEvent& e);
    /** Event triggered when client is reconfigured (size, position etc.).*/
	void onConfigureNotify  (const XConfigureEvent& e);

	void onDestroyNotify    (const XDestroyWindowEvent& e);
    /** Event triggered when mouse cursor enters client area. */
	void onEnterNotify      (const XCrossingEvent& e);
	// requests
	void onConfigureRequest (const XConfigureRequestEvent& e);
	void onMapRequest       (const XMapRequestEvent& e);
	// input
	void onKeyPress         (const XKeyEvent& e);
	void onButtonPress      (const XButtonPressedEvent& e);
	// events
	void onClientMessage    (const XClientMessageEvent& e);
	void onExpose           (const XExposeEvent& e);
	void onFocusIn          (const XFocusChangeEvent& e);

private: // utility methods

    //TODO: description
    void grabKeys();
    Client* getClientForWindow(Window W);
    std::unordered_map<Window, Client*> m_windowClientMap;

private: // variables
    bool m_running;
    xlib::XCore& m_x;

    std::list<Monitor> m_monitors;
    Monitor* m_focusedMonitorPtr;
};
