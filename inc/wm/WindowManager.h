#pragma once

#include "xlib/Xlib.h" 
#include "config/Config.h"
#include "Monitor.h"

#ifdef DEBUG_LOG
#    include <iostream>
#    define LOG(x) std::cout << (x) << std::endl;
#else 
#    define LOG(x)
#endif

class WindowManager {
public:
    WindowManager(xlib::XCore& x);
    static void configure(const config::WMConfig& conf);

    void run();

private: // functions 

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

private: // variables
    bool m_running;
    xlib::XCore& m_x;

    Monitor* m_focusedMonitorPtr;
};
