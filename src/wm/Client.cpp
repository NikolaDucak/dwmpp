#include "wm/Client.h"
#include <X11/Xlib.h>

void Client::configure(const config::ClientConfig&){

}

Client::Client(Workspace& ws, Window w, XWindowAttributes& wa) :
	workspaceRef_(&ws),
	xwin_(w),
	hidden_(false), floating_(false), urgent_(false), fullscreen_(false),
	xy_( {wa.x, wa.y} ), 
	wh_( {wa.width, wa.height} ),
	old_xy_( {wa.x, wa.y} ),
   	old_wh_( {wa.width, wa.height} )
	//bw_(config.borderWidth), old_bw_( wa.border_width)
{
	//XlibWrapper::instance().getWindowAttributes( w, wa );
}

Client::Client(Workspace& ws, Window w) :
	workspaceRef_(&ws),
	xwin_(w),
	hidden_(false), floating_(false), urgent_(false), fullscreen_(false),
	bw_(1) // TODO: take value from config
{
    
    XWindowAttributes wa;
    xwin_.getWindowAttrinbutes(&wa);
	xy_ = {wa.x, wa.y};
	wh_ = {wa.width, wa.height};
	old_xy_ = {wa.x, wa.y};
   	old_wh_ = {wa.width, wa.height};
    old_bw_ = wa.border_width;
    
}


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
    xwin_.setWindowBorder(0xffffff);
    //TODO: move send event to xwin_
    this->sendEvent(xwin_.xcore->getAtom(xlib::WMTakeFocus));
}

void Client::dropInputFocus() {
    //TODO: focus root window
    xwin_.setWindowBorder(0x0000aa);
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
    if (f == false) {
        xy_ = old_xy_;
        wh_ = old_wh_;
        resize(xy_, wh_);
    }
}

void Client::setState(int state) {
    long data[] = { state, None };
    xwin_.changeProperty(xlib::WMState, xlib::WMState, (unsigned char*)data, 2);
}

void Client::move(point xy) { resize(xy, wh_); }

void Client::resize(point wh) { resize(xy_, wh); }

void Client::raise() { xwin_.raiseWindow(); }

