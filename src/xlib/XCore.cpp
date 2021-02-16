#include "xlib/XCore.h"
#include "xlib/XError.h"


namespace xlib {

XCore& XCore::instance() {
    static XCore x(0);
    return x;
}

XCore::XCore( char * d ) :
    dpy_(XOpenDisplay(d)), 
    screen_(XDefaultScreen(dpy_)),
    root_(XRootWindow(dpy_, screen_)) 
{
    // init atoms
    m_atoms[XAWindow]        = XA_WINDOW;
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
    m_atoms[NetWMWindowTypeDialog] =
        XInternAtom(dpy_, "_NET_WM_WINDOW_TYPE_DIALOG", False);
    m_atoms[NetWMFullscreen] =
        XInternAtom(dpy_, "_NET_WM_STATE_FULLSCREEN", False);


    // TODO: maybe let upper classes have control ower NetWMNAme
    auto wmcheckwin = XCreateSimpleWindow(dpy_, root_, 0, 0, 1, 1, 0, 0, 0);
    XChangeProperty(dpy_, wmcheckwin, m_atoms[NetWMCheck], XA_WINDOW, 32,
                    PropModeReplace, (unsigned char*)&wmcheckwin, 1);
    XChangeProperty(dpy_, wmcheckwin, m_atoms[NetWMName], m_atoms[UTF8String], 8,
                    PropModeReplace, (unsigned char*)"dwmpp", 3);
    XChangeProperty(dpy_, root_, m_atoms[NetWMCheck], XA_WINDOW, 32,
                    PropModeReplace, (unsigned char*)&wmcheckwin, 1);


    // EWMH support per view 
    // m_atoms starts with net atoms
    // TODO: needing to have NetLast kinda smells
    XChangeProperty(dpy_, root_, m_atoms[NetSupported], XA_ATOM, 32,
                    PropModeReplace, (unsigned char*)m_atoms, NetLast);
    XDeleteProperty(dpy_, root_, m_atoms[NetClientList]);

    err::init();
}

Window XCore::readActiveWindowProperty() {
    int            di;
    unsigned long  dl;
    unsigned long  data_length;
    unsigned char* p = NULL;
    Atom           da;
    Window out;
    XGetWindowProperty(dpy_, root_, getAtom(NetActiveWindow), 
                       0L, sizeof(Atom), False, // TODO: sizeof may not be correct 
                       XA_WINDOW, &da, &di, &data_length, &dl, &p);
    if(data_length) {
        out = *(Window*)p;
        return out;
    } else {
        return 0;
    }
    XFree(p);
}

void XCore::sync(bool a) { 
    XSync(dpy_, a);
}

bool XCore::queryPointer(int* x, int* y) {
    int di;
    unsigned int dui;
    Window dummy;
    return XQueryPointer(dpy_, root_, &dummy, &dummy, x, y, &di, &di,
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
std::string XCore::getTextProperity(Atom atom) {
    XTextProperty name;
    std::string str;

    if (!XGetTextProperty(dpy_, root_, &name, atom) || !name.nitems)
        return { "" };
    if (name.encoding == XA_STRING)
        str = (char*)(name.value);
    else {
        char** list = NULL;
        int n;
        if (XmbTextPropertyToTextList(dpy_, &name, &list, &n) >=
                Success &&
            n > 0 && *list) {
            str = (char*)(name.value);
            XFreeStringList(list);
        }
    }
    XFree(name.value);
    return str;
}

std::string XCore::getTextProperity(AtomType at) {
    Atom atom = getAtom(at);
    return getTextProperity(atom);
}

} // namespace xlib
