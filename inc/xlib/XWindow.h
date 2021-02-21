#pragma once

#include "XCore.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <optional>

namespace xlib {

//TODO: some some things take `Atom` as argument, other use `AtomType`, thats verry incosistent.
class XWindow {
public:

    /*
     * Create new window using XCreateWindow, with root window for parent
     */
    XWindow(int x, int y, unsigned width, unsigned height);

    /*
     * Wrapper around existing window
     */
    explicit XWindow(Window w);

    void configureWindow(XWindowChanges& changes, unsigned int mask);

    Atom getAtomProperty(AtomType at) const;
    std::string getTextProperity(AtomType) const;

    std::optional<Window> getTransientFor() const ;

    std::optional<XWMHints*> getWMHints() const;
    void                     setWMHints(XWMHints& wm);

    void moveWindow(int x, int y);
    void resizeWindow(int w, int h);

    void setWindowBorder(unsigned long pixel);
    void setWindowBorderWidth(uint w);

    void setClassHint(XClassHint ch);

    void selectInput(unsigned long mask);

    void sendEvent(AtomType a);
    /** Return true if @p a is supported and event is sent */
    bool trySendEvent(AtomType a);
    void sendEvent(XEvent& ev);
    void sendEvent(XEvent& ev, int eventMask);


    void setInputFocus();
    void raiseWindow();
    void mapRaised();
    void mapWindow();
    void killClient();

    void grabButton(unsigned int button, unsigned int mask);

    void getWindowAttrinbutes(XWindowAttributes* wa) const;

    // atom related stuff
    void setFullscreen(bool fullscreen) ;
    void dropNetActiveAtom();
    void setNetActiveAtom();
    bool getWMProtocols(Atom** protocols, int* n) const;

    void changeProperty(AtomType prop, AtomType type,
                        unsigned char* data, int data_size);

    bool supportsProtocol(Atom prot) const;

    // needs to be set before class usage
    static XCore* xcore;

    // get raw window
    inline Window get() const { return m_w; }

    static void revertInputFocusToRoot() {
        XSetInputFocus(xcore->getDpyPtr(), xcore->getRoot(),
                       RevertToPointerRoot, CurrentTime);
    }

private:
    Window m_w;
};

}


