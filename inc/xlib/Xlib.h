#pragma once

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>

namespace xlib {

/*
 * All supported atom types
 */
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

class XCore {
public:
    XCore( char* );

    void sync(bool a);
    void getWindowAttributes(Window w, XWindowAttributes& wa);

    bool queryPointer(int x, int y);

    /*
     * uses XSelectInput on root window to set all inputs
     * lib can take
     */
    void selectInput(unsigned long mask);

    void grabKey(KeySym keySym, int mod);
    unsigned int keySymToKeyCode(KeySym k);

    int  grabPointer(unsigned long mousemask);
    void ungrabPointer();

    void refreshKeyboardMapping(XMappingEvent& e);
    void getTextProperity();
    Atom getAtom(AtomType a) const;

    void nextEvent(XEvent* ev);

    inline Display* getDpyPtr()    { return dpy_; }
    inline Window   getRoot()      { return root_; }
    inline int      getScreen()    { return screen_; }

private:

    Display *dpy_;
    int screen_;
    Window root_;
    Atom m_atoms[AtomLast];
};

class XGraphics {};
class XFont {};
class XColor {};

} // namespace xlib
