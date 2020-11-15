#include "wm/Client.h"

std::map<Window, Client*> Client::clientWindowMap;

Client::Client(Workspace& ws, Window w, XWindowAttributes& wa) :
	m_parentWorkspace(&ws),
	m_xWindow(w),
	m_hidden(false), m_floating(false), m_urgent(false), m_fullscreen(false),
	m_topLeft( {wa.x, wa.y} ), 
	m_dimensions( {wa.width, wa.height} ),
	m_oldTopLeft( {wa.x, wa.y} ),
   	m_oldDimensions( {wa.width, wa.height} ),
	m_borderWidth(config.borderWidth), 
    m_oldBorderWidth(wa.border_width)
{
    // select events for that window
    m_xWindow.selectInput(EnterWindowMask | 
                      FocusChangeMask | 
                      PropertyChangeMask |
                      StructureNotifyMask);

    // asing pointer to client object for this window
    clientWindowMap[m_xWindow.get()] = this;
}

Client::Client(Workspace& ws, Window w) :
	m_parentWorkspace(&ws),
	m_xWindow(w),
	m_hidden(false), m_floating(false), m_urgent(false), m_fullscreen(false),
	m_borderWidth(config.borderWidth)
{
    
    XWindowAttributes wa;
    m_xWindow.getWindowAttrinbutes(&wa);
	m_topLeft = {wa.x, wa.y};
	m_dimensions = {wa.width, wa.height};
	m_oldTopLeft = {wa.x, wa.y};
   	m_oldDimensions = {wa.width, wa.height};
    m_oldBorderWidth = wa.border_width;

    // select events for that window
    m_xWindow.selectInput(EnterWindowMask | 
                      FocusChangeMask | 
                      PropertyChangeMask |
                      StructureNotifyMask);
    
    clientWindowMap[m_xWindow.get()] = this;
}

Client::~Client() { clientWindowMap.erase(m_xWindow.get()); }

void Client::resize(point xy, point wh) {
	m_oldTopLeft = m_topLeft; 
	m_oldDimensions = m_dimensions;
	m_topLeft = xy; 
	m_dimensions = wh;

	// set window changes
	XWindowChanges wc {
		.x = xy.x,
		.y = xy.y,
		.width = wh.x,
		.height = wh.y,
		.border_width = m_borderWidth,
	};

    m_xWindow.configureWindow(wc, 
			CWX|CWY|CWWidth|CWHeight|CWBorderWidth);
		
	// config
	XConfigureEvent ce {
		.type = ConfigureNotify,
		.display = m_xWindow.xcore->getDpyPtr(),
		.event = m_xWindow.get(),
		.window = m_xWindow.get(),
		.x = xy.x,
		.y = xy.y,
		.width = wh.x,
		.height = wh.y,
		.border_width = m_borderWidth,
		.above = None,
		.override_redirect = False
	};

	//TODO: messy, clean up
    m_xWindow.sendEvent( *(XEvent*)&ce );
    m_xWindow.xcore->sync(false);
}

void Client::takeInputFocus() {
    m_xWindow.setInputFocus();
    m_xWindow.changeProperty( xlib::NetActiveWindow, xlib::XAWindow,
		reinterpret_cast<unsigned char *>(&m_xWindow), 1);
    m_xWindow.setActive();
    m_xWindow.setWindowBorder(config.selecetedBorderClr.get().pixel);
    //TODO: move send event to m_xWindow
    this->sendEvent(m_xWindow.xcore->getAtom(xlib::WMTakeFocus));
}

void Client::dropInputFocus() {
    //TODO: smells..
    XSetInputFocus(m_xWindow.xcore->getDpyPtr(), m_xWindow.xcore->getRoot(),
                   RevertToPointerRoot, CurrentTime);
    m_xWindow.setWindowBorder(config.borderClr.get().pixel);
}

void Client::hide() {
    m_hidden = true;
    m_xWindow.moveWindow(-(m_dimensions.x + 2 * 20) * 2, m_topLeft.y);
}

void Client::show() {
    m_hidden = false;
    m_xWindow.moveWindow(m_topLeft.x, m_topLeft.y);
}

bool Client::sendEvent(Atom proto) {
    // send if supported
    if (m_xWindow.supportsProtocol(proto)) {
        XEvent ev;
        ev.type                 = ClientMessage;
        ev.xclient.window       = m_xWindow.get();
        ev.xclient.message_type = m_xWindow.xcore->getAtom(xlib::WMProtocols);
        ev.xclient.format       = 32;
        ev.xclient.data.l[0]    = proto;
        ev.xclient.data.l[1]    = CurrentTime;

        m_xWindow.sendEvent(ev);

        return true;
    }
    return false;
}

void Client::setFullscreen(bool f) {
    m_fullscreen = f;
    if (m_fullscreen) {
        m_oldBorderWidth = m_borderWidth;
        m_borderWidth = 0;
        //TODO: resize handles border width
        // should be named 'reconfigure'
        resize(m_topLeft, m_dimensions);
    } else {
        m_topLeft = m_oldTopLeft;
        m_dimensions = m_oldDimensions;
        m_borderWidth = m_oldBorderWidth;
        resize(m_topLeft, m_dimensions);
    }
}

void Client::setState(int state) {
    long data[] = { state, None };
    m_xWindow.changeProperty(xlib::WMState, xlib::WMState, (unsigned char*)data, 2);
}

std::string Client::getTitle() {
    return m_xWindow.getTextProperity(xlib::NetWMName);
}

void Client::move(point xy) { resize(xy, m_dimensions); }

void Client::resize(point wh) { resize(m_topLeft, wh); }

void Client::raise() { m_xWindow.raiseWindow(); }

