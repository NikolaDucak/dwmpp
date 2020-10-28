#pragma once

#include "config/ConfigTypes.h"
#include "util/point.h"
#include "xlib/XWindow.h"

#include <list>
#include <algorithm>

class Monitor;
class Workspace;

class Client {
public:

    /*
	// Configuration
	struct Config {
		int borderWidth;
        xlib::XColor borderClr;
        xlib::XColor selecetedBorderClr;
	};
	static const Config config;
    */

    static void configure( const config::ClientConfig& conf );

public:

	Client(Workspace& ws, Window w, XWindowAttributes& wa);
	Client(Workspace& ws, Window w);

	void resize(point xy, point wh);
	void resize(point wh);
	void move  (point xy);

	bool sendEvent(Atom atom);

	void raise();

	void hide();
	void show();

	//void setBorderColor(const xlib::XColor& c);

	void setState(int s);
	void takeInputFocus();

	inline void assignToWorkspace(Workspace& ws) { workspaceRef_ = &ws; }
	inline Workspace& getWorkspace() { return *workspaceRef_; }

	inline const xlib::XWindow& getXWindow() const  { return xwin_; }

	inline const point& getPosition() const { return xy_; }
	inline const point& getSize() const     { return wh_; }
	inline int getBorderWidth() const       { return bw_; }

	inline bool isFloating() const   { return floating_; }
	inline bool isHidden() const     { return hidden_; }
	inline bool isUrgent() const     { return urgent_; }
	inline bool isFullscreen() const { return fullscreen_; }

	void setFullscreen(bool f);
	void setUrgent(bool f)       { urgent_ = f; }
	void setFloating(bool f)     { floating_ = f; }
	void toggleFloating()        { floating_ = not floating_; }
	void toggleFullscreen()      { fullscreen_ = not fullscreen_; }

private:

	Workspace *workspaceRef_;
    xlib::XWindow xwin_;
	bool hidden_, floating_, urgent_, fullscreen_;
	// saving current position and old position, for floation - tiled change
    point xy_,     wh_,      old_xy_, old_wh_; 
	// border w
	int   bw_,     old_bw_; 
};