#include "xlib/XWindow.h"
#include <X11/Xutil.h>

namespace xlib {

XCore* XWindow::xcore;

XWindow::XWindow(Window w) : m_w(w) { 
}

XWindow::XWindow(int x, int y, unsigned width, unsigned height) :
    m_w(XCreateSimpleWindow(xcore->getDpyPtr(), xcore->getRoot(), 
                x, y, width, height, 0, 0x0, 0x0)) {}

void XWindow::moveWindow(int x, int y) {
    XMoveWindow(xcore->getDpyPtr(), m_w, x, y);
}

void XWindow::resizeWindow(int w, int h) {
    XResizeWindow(xcore->getDpyPtr(), m_w, w, h);
}

void XWindow::setWindowBorder(unsigned long pixel) {
    XSetWindowBorder(xcore->getDpyPtr(), m_w, pixel);
}

void XWindow::selectInput(unsigned long int inputMask) {
    XSelectInput(xcore->getDpyPtr(), m_w, inputMask);
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

void XWindow::killClient() {
    XGrabServer(xcore->getDpyPtr());
    // XSetErrorHandler(xerrordummy);
    XSetCloseDownMode(xcore->getDpyPtr(), DestroyAll);
    XKillClient(xcore->getDpyPtr(), m_w);
    XSync(xcore->getDpyPtr(), False);
    // XSetErrorHandler(xerror);
    XUngrabServer(xcore->getDpyPtr());
}

void XWindow::grabButton(unsigned int button, unsigned int mask) {
    XGrabButton(xcore->getDpyPtr(), button, mask, m_w, false,
                ButtonPressMask | ButtonReleaseMask | ButtonMotionMask,
                GrabModeAsync, GrabModeAsync, None, None);
}

// atom related stuff

bool XWindow::getWMProtocols(Atom** protocols, int* n) {
    return XGetWMProtocols(xcore->getDpyPtr(), m_w, protocols, n);
}


void XWindow::changeProperty(Atom prop, AtomType type,
                    unsigned char* data, int data_size) {
    XChangeProperty(xcore->getDpyPtr(), m_w, prop, type, 32, PropModeReplace,
                    (unsigned char*)data, data_size);
}

void XWindow::setActive() {
    XChangeProperty(xcore->getDpyPtr(),
                    xcore->getRoot(), 
                    xcore->getAtom(NetActiveWindow),
                    XA_WINDOW, 32, PropModeReplace, 
                    (unsigned char*)&m_w, 1);
}

void XWindow::setFullscreen(bool fullscreen) {
    if (fullscreen) {
        Atom temp = xcore->getAtom(NetWMFullscreen);
        XChangeProperty(xcore->getDpyPtr(), m_w, xcore->getAtom(NetWMState), XA_ATOM, 32,
                        PropModeReplace,
                        (unsigned char*)&temp, 1);
    } else {
        XChangeProperty(xcore->getDpyPtr(), m_w, xcore->getAtom(NetWMState), XA_ATOM, 32,
                        PropModeReplace, (unsigned char*)0, 0);
    }
}

bool XWindow::supportsProtocol(Atom proto) {
	int n;
	Atom *protocols;
	bool exists = false;

	if (getWMProtocols(&protocols, &n)) {
		exists = std::find(protocols, protocols+n, proto) != protocols+n;
        XFree(protocols);
    }
    return exists;
}

std::string XWindow::getTextProperity(Atom atom) {
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

std::string XWindow::getTextProperity(AtomType at) {
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

void XWindow::getWindowAttrinbutes(XWindowAttributes* wa) {
    XGetWindowAttributes(xcore->getDpyPtr(), m_w, wa);
}

} // namespace xlib
