#include "XLibWrapper.h"

#include <X11/Xlib.h>

XLib XLib::instance_;
XLib& XLib::instance() { return instance_; }

static XLib& x_ = XLib::instance();

XLib::XLib() :
    dpy_(XOpenDisplay(0)), screen_(XDefaultScreen(dpy_)),
    root_(XRootWindow(dpy_, screen_)) {
    atom[UTF8String]      = XInternAtom(dpy_, "UTF8_STRING", False);
    atom[WMProtocols]     = XInternAtom(dpy_, "WM_PROTOCOLS", False);
    atom[WMDelete]        = XInternAtom(dpy_, "WM_DELETE_WINDOW", False);
    atom[WMState]         = XInternAtom(dpy_, "WM_STATE", False);
    atom[WMTakeFocus]     = XInternAtom(dpy_, "WM_TAKE_FOCUS", False);
    atom[NetActiveWindow] = XInternAtom(dpy_, "_NET_ACTIVE_WINDOW", False);
    atom[NetSupported]    = XInternAtom(dpy_, "_NET_SUPPORTED", False);
    atom[NetWMName]       = XInternAtom(dpy_, "_NET_WM_NAME", False);
    atom[NetWMState]      = XInternAtom(dpy_, "_NET_WM_STATE", False);
    atom[NetWMCheck] = XInternAtom(dpy_, "_NET_SUPPORTING_WM_CHECK", False);
    atom[NetWMWindowType] = XInternAtom(dpy_, "_NET_WM_WINDOW_TYPE", False);
    atom[NetClientList] = XInternAtom(dpy_, "_NET_CLIENT_LIST", False);
    atom[XAWindow]      = XA_WINDOW;
    atom[NetWMWindowTypeDialog] =
        XInternAtom(dpy_, "_NET_WM_WINDOW_TYPE_DIALOG", False);
    atom[NetWMFullscreen] =
        XInternAtom(dpy_, "_NET_WM_STATE_FULLSCREEN", False);
}

void XLib::selectInput(unsigned long mask) { XSelectInput(dpy_, root_, mask); }

void XLib::nextEvent(XEvent& return_event) { XNextEvent(dpy_, &return_event); }

void XLib::mapWindow(Window w) { XMapWindow(dpy_, w); }

void XLib::sync(bool a) { XSync(dpy_, a); }

void XLib::selectInput(Window w, unsigned long mask) {
    XSelectInput(dpy_, w, mask);
}


void XLib::configureWindow(Window w, XWindowChanges& changes,
                           unsigned int mask) {
    XConfigureWindow(dpy_, w, mask, &changes);
}

bool XLib::queryPointer(point p) {
    int di;
    unsigned int dui;
    Window dummy;

    return XQueryPointer(dpy_, root_, &dummy, &dummy, &p.x, &p.y, &di, &di,
                         &dui);
}

void XLib::grabKey(KeySym keySym, int mod) {
    XGrabKey(dpy_, XKeysymToKeycode(dpy_, keySym), mod, root_, True,
             GrabModeAsync, GrabModeAsync);
}

void XLib::moveWindow(Window w, int x, int y) { XMoveWindow(dpy_, w, x, y); }

void XLib::grabButton(Window w, unsigned int button, unsigned int mask) {
    XGrabButton(dpy_, button, mask, w, false,
                ButtonPressMask | ButtonReleaseMask | ButtonMotionMask,
                GrabModeAsync, GrabModeAsync, None, None);
}

void XLib::killClient(Window w) {
    XGrabServer(dpy_);
    // XSetErrorHandler(xerrordummy);
    XSetCloseDownMode(dpy_, DestroyAll);
    XKillClient(dpy_, w);
    XSync(dpy_, False);
    // XSetErrorHandler(xerror);
    XUngrabServer(dpy_);
}

int XLib::grabPointer(unsigned long mousemask) {
    return XGrabPointer(dpy_, root_, False, mousemask, GrabModeAsync,
                        GrabModeAsync, None, None, CurrentTime);
}

void XLib::ungrabPointer() { XUngrabPointer(dpy_, CurrentTime); }

void XLib::maskEvent(XEvent& ev, unsigned long mask) {
    XMaskEvent(dpy_, mask, &ev);
}

void XLib::raiseWindow(Window w) { XRaiseWindow(dpy_, w); }

void XLib::sendEvent(Window w, XEvent& ev, int eventMask) {
    XSendEvent(dpy_, w, False, eventMask, &ev);
}

const Atom& XLib::getAtom(AtomType a) { return atom[a]; }

void XLib::sendEvent(Window w, XEvent& ev) {
    XSendEvent(dpy_, w, False, NoEventMask, &ev);
}

void XLib::getWindowAttributes(Window w, XWindowAttributes& wa) {
    XGetWindowAttributes(dpy_, w, &wa);
}

bool XLib::getWMProtocosl(Window w, Atom** protocols, int* n) {
    return XGetWMProtocols(dpy_, w, protocols, n);
}

void XLib::changeProperty(Window w, AtomType prop, AtomType type,
                          unsigned char* data, int data_size) {
    XChangeProperty(dpy_, w, prop, type, 32, PropModeReplace,
                    (unsigned char*)data, data_size);
}

void XLib::setInputFocus(Window w) {
    XSetInputFocus(dpy_, w, RevertToPointerRoot, CurrentTime);
}

unsigned int XLib::keySymToKeyCode(KeySym k) {
    return XKeysymToKeycode(dpy_, k);
}

void XLib::setWindowBorder(Window w, unsigned long pixel) {
    XSetWindowBorder(dpy_, w, pixel);
}

XGraphics::XGraphics(XLib& x) : dpy_(x.getDpy()) {
    drawable_ = XCreatePixmap(dpy_, x.getRoot(),
                              DisplayWidth(x.getDpy(), x.getScreen()),
                              DisplayHeight(x.getDpy(), x.getScreen()),
                              DefaultDepth(x.getDpy(), x.getScreen()));
    gc_       = XCreateGC(dpy_, x.getRoot(), 0, NULL);
    XSetLineAttributes(dpy_, gc_, 1, LineSolid, CapButt, JoinMiter);
}

void XGraphics::fillRectangle(const Color& c, point xy, point wh) {
    XSetForeground(dpy_, gc_, c.xColor().pixel);
    XFillRectangle(dpy_, drawable_, gc_, xy.x, xy.y, wh.x, wh.y);
}

void XGraphics::copyArea(Window w, point xy, point wh) {
    XCopyArea(dpy_, drawable_, w, gc_, xy.x, xy.y, wh.x, wh.y, xy.x, xy.y);
    XSync(dpy_, False);
}

void XGraphics::drawText(Window w, XFont fnt, Color clr, point xy,
                         const std::string& text) {
    int scr       = DefaultScreen(dpy_);
    int ty        = xy.y + fnt.getXFont()->ascent;  // correct text position
    XftDraw* draw = XftDrawCreate(dpy_, w, DefaultVisual(dpy_, scr),
                                  DefaultColormap(dpy_, scr));
    XftDrawStringUtf8(draw, &clr.xColor(), fnt.getXFont(), xy.x, ty,
                      (const FcChar8*)text.c_str(), text.size());
    XftDrawDestroy(draw);
}

Color::Color(std::string clrname) {
    auto dpy = XLib::instance().getDpy();
    if (!XftColorAllocName(dpy, DefaultVisual(dpy, DefaultScreen(dpy)),
                           DefaultColormap(dpy, DefaultScreen(dpy)),
                           clrname.c_str(), &color_))
        util::die("error, cannot allocate color ");
}

Color::Color(Display* dpy, std::string clrname) {
    if (!XftColorAllocName(dpy, DefaultVisual(dpy, DefaultScreen(dpy)),
                           DefaultColormap(dpy, DefaultScreen(dpy)),
                           clrname.c_str(), &color_))
        util::die("error, cannot allocate color ");
}

/* Using the pattern found at font->xfont->pattern does not yield the
 * same substitution results as using the pattern returned by
 * FcNameParse; using the latter results in the desired fallback
 * behaviour whereas the former just results in missing-character
 * rectangles being drawn, at least with some fonts. */
XFont::XFont(const std::string& fontname) :
    xfont_(XftFontOpenName(x_.getDpy(), x_.getScreen(), fontname.c_str())),
    pattern_(FcNameParse(reinterpret_cast<const FcChar8*>(fontname.c_str()))) {
    if (not xfont_) {
        util::die("XFONT ERR: cant load form name %s", fontname.c_str());
    }
    if (not pattern_) {
        XftFontClose(x_.getDpy(), xfont_);
        util::die("XFONT ERR: cant load name form pattern %s",
                  fontname.c_str());
    }

    /* Do not allow using color fonts. This is a workaround for a BadLength
     * error from Xft with color glyphs. Modelled on the Xterm workaround. See
     * https://bugzilla.redhat.com/show_bug.cgi?id=1498269
     * https://lists.suckless.org/dev/1701/30932.html
     * https://bugs.debian.org/cgi-bin/bugreport.cgi?bug=916349
     * and lots more all over the internet.
     */
    FcBool iscol;
    if (FcPatternGetBool(xfont_->pattern, FC_COLOR, 0, &iscol) ==
            FcResultMatch &&
        iscol) {
        XftFontClose(x_.getDpy(), xfont_);
        util::die("DIE no gliph");
    }
    this->height_ = xfont_->ascent + xfont_->descent;
}

int XFont::getTextWidthInPixels(const std::string& text) const {
    XGlyphInfo ext;
    XftTextExtentsUtf8(x_.getDpy(), xfont_, (XftChar8*)text.c_str(),
                       text.size(), &ext);
    return ext.xOff;
}

std::string XAtom::getTextProperity(Window w, Atom atom) {
    XTextProperty name;
    std::string str;

    if (!XGetTextProperty(x.getDpy(), w, &name, atom) || !name.nitems)
        return { "" };
    if (name.encoding == XA_STRING)
        str = (char*)(name.value);
    else {
        char** list = NULL;
        int n;
        if (XmbTextPropertyToTextList(x.getDpy(), &name, &list, &n) >=
                Success &&
            n > 0 && *list) {
            str = (char*)(name.value);
            XFreeStringList(list);
        }
    }
    XFree(name.value);
    return str;
}

void XAtom::deleteClientList() {
    XDeleteProperty(x.getDpy(), x.getRoot(), x.getAtom(NetClientList));
}

void XAtom::appendToClientList(Window w) {
    XChangeProperty(x.getDpy(), x.getRoot(), x.getAtom(NetClientList),
                    XA_WINDOW, 32, PropModeAppend, (unsigned char*)&w, 1);
}

void XAtom::setActive(Window w) {
    XChangeProperty(x.getDpy(), x.getRoot(), x.getAtom(NetActiveWindow),
                    XA_WINDOW, 32, PropModeReplace, (unsigned char*)&w, 1);
}

void XAtom::removeActive() {
    XDeleteProperty(x.getDpy(), x.getRoot(), x.getAtom(NetActiveWindow));
}

void XAtom::setFullscreen(Window w, bool fullscreen) {
    if (fullscreen)
        XChangeProperty(x.getDpy(), w, x.getAtom(NetWMState), XA_ATOM, 32,
                        PropModeReplace,
                        (unsigned char*)&x.getAtom(NetWMFullscreen), 1);
    else
        XChangeProperty(x.getDpy(), w, x.getAtom(NetWMState), XA_ATOM, 32,
                        PropModeReplace, (unsigned char*)0, 0);
}
}

static auto& x = XlibWrapper::instance();
namespace xatom {
std::string getTextProperity(Window w, Atom atom) {
    XTextProperty name;
    std::string str;

    if (!XGetTextProperty(x.getDpy(), w, &name, atom) || !name.nitems)
        return { "" };
    if (name.encoding == XA_STRING)
        str = (char*)(name.value);
    else {
        char** list = NULL;
        int n;
        if (XmbTextPropertyToTextList(x.getDpy(), &name, &list, &n) >=
                Success &&
            n > 0 && *list) {
            str = (char*)(name.value);
            XFreeStringList(list);
        }
    }
    XFree(name.value);
    return str;
}

void deleteClientList() {
    XDeleteProperty(x.getDpy(), x.getRoot(), x.getAtom(NetClientList));
}

void appendToClientList(Window w) {
    XChangeProperty(x.getDpy(), x.getRoot(), x.getAtom(NetClientList),
                    XA_WINDOW, 32, PropModeAppend, (unsigned char*)&w, 1);
}

void setActive(Window w) {
    XChangeProperty(x.getDpy(), x.getRoot(), x.getAtom(NetActiveWindow),
                    XA_WINDOW, 32, PropModeReplace, (unsigned char*)&w, 1);
}

void removeActive() {
    XDeleteProperty(x.getDpy(), x.getRoot(), x.getAtom(NetActiveWindow));
}

void setFullscreen(Window w, bool fullscreen) {
    if (fullscreen)
        XChangeProperty(x.getDpy(), w, x.getAtom(NetWMState), XA_ATOM, 32,
                        PropModeReplace,
                        (unsigned char*)&x.getAtom(NetWMFullscreen), 1);
    else
        XChangeProperty(x.getDpy(), w, x.getAtom(NetWMState), XA_ATOM, 32,
                        PropModeReplace, (unsigned char*)0, 0);
}

}  // namespace xatom

namespace xutil {

unsigned int cleanMask(unsigned int mask) {
    return (mask & ~(xutil::numlockmask | LockMask) &
            (ShiftMask | ControlMask | Mod1Mask | Mod2Mask | Mod3Mask |
             Mod4Mask | Mod5Mask));
}

inline void updateNumlockMask() {
    int i, j;
    XModifierKeymap* modmap;
    xutil::numlockmask = 0;
    modmap             = XGetModifierMapping(x.getDpy());
    for (i = 0; i < 8; i++)
        for (j = 0; j < (int)modmap->max_keypermod; j++)
            if (modmap->modifiermap[i * modmap->max_keypermod + j] ==
                XKeysymToKeycode(x.getDpy(), XK_Num_Lock))
                numlockmask = (1 << i);
    XFreeModifiermap(modmap);
}
}  // namespace xutil

