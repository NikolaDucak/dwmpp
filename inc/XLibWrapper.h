#pragma once

#include "Point.h"
#include "util.h"

#include <X11/Xatom.h>
#include <X11/Xft/Xft.h>
#include <X11/Xlib.h>
#include <X11/Xproto.h>
#include <X11/Xutil.h>
#include <stdint.h>
#include <string>

enum AtomType {
    NetSupported,
    NetWMName,
    NetWMCheck,
    NetWMState,
    NetWMFullscreen,
    NetActiveWindow,
    NetWMWindowType,
    NetWMWindowTypeDialog,
    NetClientList,
    WMProtocols,
    WMDelete,
    WMState,
    WMTakeFocus,
    XAWindow,
    UTF8String,
    AtomLast
};

class XLib {
private:
    static XLib instance_;

public:
    static XLib& instance();

    XLib();

    void moveWindow(Window w, int x, int y);
    void setWindowBorder(Window w, unsigned long pixel);

    void selectInput(unsigned long mask);
    void selectInput(Window w, unsigned long mask);

    void sendEvent(Window w, XEvent& ev);
    void sendEvent(Window w, XEvent& ev, int eventMask);
    void nextEvent(XEvent& return_event);
    void maskEvent(XEvent& ev, unsigned long mask);

    void sync(bool a);

    void changeProperty(Window w, AtomType prop, AtomType type,
                        unsigned char* data, int data_size);
    bool getWMProtocosl(Window w, Atom** protocols, int* n);
    void setInputFocus(Window w);
    void configureWindow(Window w, XWindowChanges& changes, unsigned int mask);
    void raiseWindow(Window w);
    void mapWindow(Window w);
    void grabButton(Window w, unsigned int button, unsigned int mask);
    void killClient(Window w);
    void getWindowAttributes(Window w, XWindowAttributes& wa);

    void grabKey(KeySym keySym, int mod);
    unsigned int keySymToKeyCode(KeySym k);

    bool queryPointer(point p);
    int grabPointer(unsigned long mousemask);
    void ungrabPointer();

    const Atom& getAtom(AtomType a);
    Display* getDpy() { return dpy_; }
    Window getRoot() { return root_; }
    Window getScreen() { return screen_; }

    // unimplemented
    void XRefreshKeyboardMapping(XEvent& e);
    void getTextProperity();

private:
    Display* dpy_;
    int screen_;
    Window root_;
    Atom atom[AtomLast];
};

/*
 * Class that relies on xlib wrapper to provide easy manipulation of
 */
class XAtom {
public:
    XAtom();

    void deleteClientList();
    void appendToClientList(Window w);
    void setActive(Window w);
    void removeActive();
    void setFullscreen(Window w, bool fullscreen);
    std::string getTextProperity(Window w, Atom atom);

private:
    XLib& x_;
};

/*===================================================================
 *                            Graphics
 *===================================================================*/

class Color {
public:
    Color(std::string clrname);
    Color(Display* dpy, std::string clrname);

    const XftColor& xColor() const { return color_; }

private:
    XftColor color_;
};

class XFont {
public:
    XFont(FcPattern* fontpattern);
    XFont(const std::string& fontname);

    int getTextWidthInPixels(const std::string& text) const;
    XftFont* getXFont() { return xfont_; }
    int getHeight() const { return height_; }

private:
    XftFont* xfont_;
    FcPattern* pattern_;
    int height_;
};

class XCursor {
    enum Type { Normal, Resize, Move, Last }; /* cursor */
public:
    XCursor(Type t);

private:
    Cursor curs_;
};

class XGraphics {
public:
    XGraphics(XLib& xlib);

    void drawText(Window w, XFont fnt, Color clr, point xy,
                  const std::string& text);
    void fillRectangle(const Color& c, point xy, point wh);
    void copyArea(Window w, point xy, point wh);

private:

    Pixmap drawable_;
    GC gc_;
    Display* dpy_;
};

// util

namespace xutil {

static unsigned int numlockmask;
inline void updateNumlockMask();
unsigned int cleanMask(unsigned int mask);

}  // namespace xutil

namespace xatom {

std::string getTextProperity(Window w, Atom atom);
void deleteClientList();
void appendToClientList(Window w);
void setActive(Window w);
void removeActive();
void setFullscreen(Window w, bool fullscreen);

}  // namespace xatom

