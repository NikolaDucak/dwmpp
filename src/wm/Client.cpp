#include "wm/Client.h"

std::map<Window, Client*> Client::clientWindowMap;

Client::Client(Workspace& ws, Window w, XWindowAttributes& wa) :
	workspaceRef_(&ws),
	xwin_(w),
	hidden_(false), floating_(false), urgent_(false), fullscreen_(false),
	xy_( {wa.x, wa.y} ), 
	wh_( {wa.width, wa.height} ),
	old_xy_( {wa.x, wa.y} ),
   	old_wh_( {wa.width, wa.height} ),
	bw_(config.borderWidth), old_bw_( wa.border_width)
{
    // select events for that window
    xwin_.selectInput(EnterWindowMask | 
                      FocusChangeMask | 
                      PropertyChangeMask |
                      StructureNotifyMask);
    clientWindowMap[xwin_.get()] = this;
}

Client::Client(Workspace& ws, Window w) :
	workspaceRef_(&ws),
	xwin_(w),
	hidden_(false), floating_(false), urgent_(false), fullscreen_(false),
	bw_(config.borderWidth) 
{
    
    XWindowAttributes wa;
    xwin_.getWindowAttrinbutes(&wa);
	xy_ = {wa.x, wa.y};
	wh_ = {wa.width, wa.height};
	old_xy_ = {wa.x, wa.y};
   	old_wh_ = {wa.width, wa.height};
    old_bw_ = wa.border_width;

    // select events for that window
    xwin_.selectInput(EnterWindowMask | 
                      FocusChangeMask | 
                      PropertyChangeMask |
                      StructureNotifyMask);
    
    clientWindowMap[xwin_.get()] = this;
}

Client::~Client() { clientWindowMap.erase(xwin_.get()); }

void Client::resize(point xy, point wh) {
	old_xy_ = xy_; 
	old_wh_ = wh_;
	xy_ = xy; 
	wh_ = wh;

	// set window changes
	XWindowChanges wc {
		.x = xy.x,
		.y = xy.y,
		.width = wh.x,
		.height = wh.y,
		.border_width = bw_,
	};

    xwin_.configureWindow(wc, 
			CWX|CWY|CWWidth|CWHeight|CWBorderWidth);
		
	// config
	XConfigureEvent ce {
		.type = ConfigureNotify,
		.display = xwin_.xcore->getDpyPtr(),
		.event = xwin_.get(),
		.window = xwin_.get(),
		.x = xy.x,
		.y = xy.y,
		.width = wh.x,
		.height = wh.y,
		.border_width = bw_,
		.above = None,
		.override_redirect = False
	};

	//TODO: messy, clean up
    xwin_.sendEvent( *(XEvent*)&ce );
    xwin_.xcore->sync(false);
}

void Client::takeInputFocus() {
    xwin_.setInputFocus();
    xwin_.changeProperty( xlib::NetActiveWindow, xlib::XAWindow,
		reinterpret_cast<unsigned char *>(&xwin_), 1);
    xwin_.setActive();
    xwin_.setWindowBorder(config.selecetedBorderClr.get().pixel);
    //TODO: move send event to xwin_
    this->sendEvent(xwin_.xcore->getAtom(xlib::WMTakeFocus));
}

void Client::dropInputFocus() {
    //TODO: smells..
    XSetInputFocus(xwin_.xcore->getDpyPtr(), xwin_.xcore->getRoot(),
                   RevertToPointerRoot, CurrentTime);
    xwin_.setWindowBorder(config.borderClr.get().pixel);
}

void Client::hide() {
    hidden_ = true;
    xwin_.moveWindow(-(wh_.x + 2 * 20) * 2, xy_.y);
}

void Client::show() {
    hidden_ = false;
    xwin_.moveWindow(xy_.x, xy_.y);
}

bool Client::sendEvent(Atom proto) {
    // send if supported
    if (xwin_.supportsProtocol(proto)) {
        XEvent ev;
        ev.type                 = ClientMessage;
        ev.xclient.window       = xwin_.get();
        ev.xclient.message_type = xwin_.xcore->getAtom(xlib::WMProtocols);
        ev.xclient.format       = 32;
        ev.xclient.data.l[0]    = proto;
        ev.xclient.data.l[1]    = CurrentTime;

        xwin_.sendEvent(ev);

        return true;
    }
    return false;
}

void Client::setFullscreen(bool f) {
    fullscreen_ = f;
    if (fullscreen_) {
        old_bw_ = bw_;
        bw_ = 0;
        //TODO: resize handles border width
        // should be named 'reconfigure'
        resize(xy_, wh_);
    } else {
        xy_ = old_xy_;
        wh_ = old_wh_;
        bw_ = old_bw_;
        resize(xy_, wh_);
    }
}

void Client::setState(int state) {
    long data[] = { state, None };
    xwin_.changeProperty(xlib::WMState, xlib::WMState, (unsigned char*)data, 2);
}

std::string Client::getTitle() {
    return xwin_.getTextProperity(xlib::NetWMName);
}

void Client::move(point xy) { resize(xy, wh_); }

void Client::resize(point wh) { resize(xy_, wh); }

void Client::raise() { xwin_.raiseWindow(); }

