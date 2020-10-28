#include "xlib/Xlib.h"

namespace xlib {

XCore::XCore( char * d ) :
    dpy_(XOpenDisplay(d)), 
    screen_(XDefaultScreen(dpy_)),
    root_(XRootWindow(dpy_, screen_)) 
{
    m_atoms[UTF8String]      = XInternAtom(dpy_, "UTF8_STRING", False);
    m_atoms[WMProtocols]     = XInternAtom(dpy_, "WM_PROTOCOLS", False);
    m_atoms[WMDelete]        = XInternAtom(dpy_, "WM_DELETE_WINDOW", False);
    m_atoms[WMState]         = XInternAtom(dpy_, "WM_STATE", False);
    m_atoms[WMTakeFocus]     = XInternAtom(dpy_, "WM_TAKE_FOCUS", False);
    m_atoms[NetActiveWindow] = XInternAtom(dpy_, "_NET_ACTIVE_WINDOW", False);
    m_atoms[NetSupported]    = XInternAtom(dpy_, "_NET_SUPPORTED", False);
    m_atoms[NetWMName]       = XInternAtom(dpy_, "_NET_WM_NAME", False);
    m_atoms[NetWMState]      = XInternAtom(dpy_, "_NET_WM_STATE", False);
    m_atoms[NetWMCheck]      = XInternAtom(dpy_, "_NET_SUPPORTING_WM_CHECK", False);
    m_atoms[NetWMWindowType] = XInternAtom(dpy_, "_NET_WM_WINDOW_TYPE", False);
    m_atoms[NetClientList]   = XInternAtom(dpy_, "_NET_CLIENT_LIST", False);
    m_atoms[XAWindow]        = XA_WINDOW;
    m_atoms[NetWMWindowTypeDialog] =
        XInternAtom(dpy_, "_NET_WM_WINDOW_TYPE_DIALOG", False);
    m_atoms[NetWMFullscreen] =
        XInternAtom(dpy_, "_NET_WM_STATE_FULLSCREEN", False);
}

void XCore::sync(bool a) { 
    XSync(dpy_, a);
}

bool XCore::queryPointer(int x, int y) {
    int di;
    unsigned int dui;
    Window dummy;
    return XQueryPointer(dpy_, root_, &dummy, &dummy, &x, &y, &di, &di,
                         &dui);
}

void XCore::selectInput(unsigned long mask) {
    XSelectInput(dpy_, root_, mask);
}

void XCore::grabKey(KeySym keySym, int mod) {
    root_ = XRootWindow(dpy_, screen_);
    XGrabKey(dpy_, XKeysymToKeycode(dpy_, keySym), mod, root_, True,
             GrabModeAsync, GrabModeAsync);
}

unsigned int XCore::keySymToKeyCode(KeySym k) {
    return XKeysymToKeycode(dpy_, k);
}

int  XCore::grabPointer(unsigned long mousemask) {
    return XGrabPointer(dpy_, root_, False, mousemask, GrabModeAsync,
                        GrabModeAsync, None, None, CurrentTime);
}

void XCore::ungrabPointer() {
    XUngrabPointer(dpy_, CurrentTime); 
}

void XCore::refreshKeyboardMapping(XMappingEvent& e) {
    XRefreshKeyboardMapping(&e);
}

void XCore::nextEvent(XEvent* ev) {
    XNextEvent(dpy_, ev);
}

Atom XCore::getAtom(AtomType type) const {
    return m_atoms[type];
}

} // namespace xlib
