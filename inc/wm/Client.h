#pragma once

#include "config/ConfigTypes.h"
#include "util/point.h"
#include "xlib/XWindow.h"
#include "xlib/XColor.h"
#include <sstream>

#include <list>
#include <map>
#include <algorithm>
#include <iostream>

class Monitor;
class Workspace;

/*
 * Class providing easy manipulation of xlib::XWindow for 
 * the puropses of window management. Functionality usefull for
 * client management includes: resizing, moving, border color &
 * width manipulation, floating/fullscreen/hidden/urgent status.
 */
class Client {
public:

	// Configuration
	struct Config {
		int borderWidth;
        xlib::XColor borderClr;
        xlib::XColor selecetedBorderClr;
	};

	static const Config config;

    // utility map used for easily finding wm::Client object
    // when wm::WindowManager receives an event and needs
    // to find map
    static std::map<Window, Client*> clientWindowMap;

public:

	Client(Workspace& ws, Window w, XWindowAttributes& wa);
	Client(Workspace& ws, Window w);
    ~Client();

    // NOTE: clientWindowMap relies on client pointers for
    // each xlib window, so having clinets moving in memory
    // complicates things;
    Client(Client&&) = delete;
    Client(Client&) = delete;
    Client& operator = (Client&) = delete;
    Client& operator = (Client&&) = delete;

	void resize(point xy, point wh);
	void resize(point wh);
	void move  (point xy);

	bool sendEvent(Atom atom);

    std::string getTitle();
	void raise();

	void hide();
	void show();

	void setState(int s);
	void takeInputFocus();
    void dropInputFocus();

	inline void assignToWorkspace(Workspace& ws) { m_parentWorkspace = &ws; }
	inline Workspace& getWorkspace() { return *m_parentWorkspace; }

	inline xlib::XWindow& getXWindow() { return m_xWindow; }
	inline const xlib::XWindow& getXWindow() const  { return m_xWindow; }

	inline const point& getPosition() const { return m_topLeft; }
	inline const point& getSize() const     { return m_dimensions; }
	inline int getBorderWidth() const       { return m_borderWidth; }

	inline bool isFloating() const   { return m_floating; }
	inline bool isHidden() const     { return m_hidden; }
	inline bool isUrgent() const     { return m_urgent; }
	inline bool isFullscreen() const { return m_fullscreen; }

	void setFullscreen(bool f);
	void setUrgent(bool f)       { m_urgent = f; }
	void setFloating(bool f)     { m_floating = f; }
	void toggleFloating()        { m_floating = not m_floating; }
	void toggleFullscreen()      { m_fullscreen = not m_fullscreen; }

private:

	Workspace *m_parentWorkspace;
    xlib::XWindow m_xWindow;
    bool m_hidden, m_floating, m_urgent, m_fullscreen;

    point m_topLeft, m_dimensions;
    point m_oldTopLeft, m_oldDimensions;
    int   m_borderWidth, m_oldBorderWidth;
};
