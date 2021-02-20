#include "xlib/XWindow.h"
#include "xlib/XError.h"
#include <algorithm>

namespace xlib {

XCore* XWindow::xcore = &XCore::instance();

XWindow::XWindow(Window w) : m_w{ w } { }

XWindow::XWindow(int x, int y, unsigned width, unsigned height) :
    m_w(XCreateSimpleWindow(xcore->getDpyPtr(), xcore->getRoot(), 
                x, y, width, height, 0, 0x0, 0x0)) {}

std::optional<Window> XWindow::getTransientFor() {
    Window trans;
    if (XGetTransientForHint(xcore->getDpyPtr(), m_w, &trans))
        return trans;
    else 
        return {};
}

void XWindow::moveWindow(int x, int y) {
    XMoveWindow(xcore->getDpyPtr(), m_w, x, y);
}

void XWindow::resizeWindow(int w, int h) {
    XResizeWindow(xcore->getDpyPtr(), m_w, w, h);
}

void XWindow::setWindowBorderWidth(uint w) {
    XSetWindowBorderWidth(xcore->getDpyPtr(), m_w, w);
}

void XWindow::setWindowBorder(unsigned long pixel) {
    XSetWindowBorder(xcore->getDpyPtr(), m_w, pixel);
}

void XWindow::selectInput(unsigned long int inputMask) {
    XSelectInput(xcore->getDpyPtr(), m_w, inputMask);
}

Atom XWindow::getAtomProperty(AtomType at) {
    int            di;
    unsigned long  dl;
    unsigned char* p = NULL;
    Atom           da, atom = None;

    if (XGetWindowProperty(xcore->getDpyPtr(), m_w, xcore->getAtom(at), 0L, sizeof(atom), False, XA_ATOM,
                           &da, &di, &dl, &dl, &p) == Success &&
        p) {
        atom = *(Atom*)p;
        XFree(p);
    }
    return atom;
}

void XWindow::sendEvent(AtomType a) {
    XEvent ev;
    ev.type                 = ClientMessage;
    ev.xclient.window       = m_w;
    ev.xclient.message_type = xcore->getAtom(xlib::WMProtocols);
    ev.xclient.format       = 32;
    ev.xclient.data.l[0]    = a;
    ev.xclient.data.l[1]    = CurrentTime;

    sendEvent(ev);
}

bool XWindow::trySendEvent(AtomType a) {
    Atom at = xcore->getAtom(a); 
    if (supportsProtocol(at)) {
        XEvent ev;
        ev.type                 = ClientMessage;
        ev.xclient.window       = m_w;
        ev.xclient.message_type = xcore->getAtom(xlib::WMProtocols);
        ev.xclient.format       = 32;
        ev.xclient.data.l[0]    = at;
        ev.xclient.data.l[1]    = CurrentTime;

        sendEvent(ev);
        return true;
    }
    return false;
}

void XWindow::sendEvent(XEvent& ev) {
    XSendEvent(xcore->getDpyPtr(), m_w, False, NoEventMask, &ev);
}

void XWindow::sendEvent(XEvent& ev, int eventMask) {
    XSendEvent(xcore->getDpyPtr(), m_w, False, eventMask, &ev);
}

void XWindow::setInputFocus() {
    XSetInputFocus(xcore->getDpyPtr(), m_w, RevertToPointerRoot, CurrentTime);
}

void XWindow::setClassHint(XClassHint ch) {
	XSetClassHint(xcore->getDpyPtr(), m_w, &ch);
}

void XWindow::mapWindow() {
    XMapWindow(xcore->getDpyPtr(), m_w);
}

void XWindow::mapRaised() {
	XMapRaised(xcore->getDpyPtr(), m_w);
}

void XWindow::raiseWindow() {
    XRaiseWindow(xcore->getDpyPtr(), m_w);
}

void XWindow::configureWindow(XWindowChanges& changes, unsigned int mask) {
    XConfigureWindow(xcore->getDpyPtr(), m_w, mask, &changes);
}

int d(Display*, XErrorEvent*) {
    return 0;
}

void XWindow::killClient() {
    auto dpy = xcore->getDpyPtr();
    XGrabServer(dpy);
    err::use_dummy_err_handler(); 
    XSetCloseDownMode(dpy, DestroyAll);
    XKillClient(dpy, m_w);
    xcore->sync(False);
    err::use_original_err_handler();
    XUngrabServer(dpy);
}

void XWindow::grabButton(unsigned int button, unsigned int mask) {
    XGrabButton(xcore->getDpyPtr(), button, mask, m_w, false,
                ButtonPressMask | ButtonReleaseMask | ButtonMotionMask,
                GrabModeAsync, GrabModeAsync, None, None);
}

// atom related stuff

bool XWindow::getWMProtocols(Atom** protocols, int* n) const {
    return XGetWMProtocols(xcore->getDpyPtr(), m_w, protocols, n);
}


std::optional<XWMHints*> XWindow::getWMHints() {
    auto wmh = XGetWMHints(xcore->getDpyPtr(), m_w);
    if (wmh) 
        return wmh;
    else 
        return {};
    
}

void XWindow::setWMHints(XWMHints& wm) {
    XSetWMHints(xcore->getDpyPtr(), m_w, &wm);
}

//TODO: atom type is gonna cause some errors...
void XWindow::changeProperty(AtomType prop, AtomType type,
                    unsigned char* data, int data_size) {
    auto prop_ = xcore->getAtom(prop);
    auto type_ = xcore->getAtom(type);
    XChangeProperty(xcore->getDpyPtr(), m_w, prop_, type_, 32, PropModeReplace,
                    (unsigned char*)data, data_size);
}

void XWindow::dropNetActiveAtom() {
    XChangeProperty(xcore->getDpyPtr(),
                    xcore->getRoot(), 
                    xcore->getAtom(NetActiveWindow),
                    XA_WINDOW, 32, PropModeReplace, 
                    (unsigned char*)0, 0);
}

void XWindow::setNetActiveAtom() {
    XChangeProperty(xcore->getDpyPtr(),
                    xcore->getRoot(), 
                    xcore->getAtom(NetActiveWindow),
                    XA_WINDOW, 32, PropModeReplace, 
                    (unsigned char*)&m_w, 2);
}

void XWindow::setFullscreen(bool fullscreen) {
    if (fullscreen) {
        Atom temp = xcore->getAtom(NetWMFullscreen);
        XChangeProperty(xcore->getDpyPtr(), m_w, xcore->getAtom(NetWMState), XA_ATOM, 32,
                        PropModeReplace, (unsigned char*)&temp, 1);
    } else {
        XChangeProperty(xcore->getDpyPtr(), m_w, xcore->getAtom(NetWMState), XA_ATOM, 32,
                        PropModeReplace, (unsigned char*)0, 0);
    }
}

bool XWindow::supportsProtocol(Atom proto) const {
	int n;
	Atom *protocols;
	bool exists = false;

	if (getWMProtocols(&protocols, &n)) {
		exists = std::find(protocols, protocols+n, proto) != protocols+n;
        XFree(protocols);
    }
    return exists;
}

std::string XWindow::getTextProperity(AtomType at) const {
    Atom atom = xcore->getAtom(at);
    XTextProperty name;
    std::string str;

    if (!XGetTextProperty(xcore->getDpyPtr(), m_w, &name, atom) || !name.nitems)
        return { "" };
    if (name.encoding == XA_STRING)
        str = (char*)(name.value);
    else {
        char** list = NULL;
        int n;
        if (XmbTextPropertyToTextList(xcore->getDpyPtr(), &name, &list, &n) >=
                Success &&
            n > 0 && *list) {
            str = (char*)(name.value);
            XFreeStringList(list);
        }
    }
    XFree(name.value);
    return str;
}

void XWindow::getWindowAttrinbutes(XWindowAttributes* wa) const {
    XGetWindowAttributes(xcore->getDpyPtr(), m_w, wa);
}

} // namespace xlib
