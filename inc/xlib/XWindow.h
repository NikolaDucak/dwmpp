#pragma once

#include "XCore.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>


#include <algorithm>

namespace xlib {

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

    Atom getAtomProperty(AtomType at);

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
    void configureWindow(XWindowChanges& changes, unsigned int mask);

    void setInputFocus();
    void raiseWindow();
    void mapWindow();
    void mapRaised();
    void killClient();

    void grabButton(unsigned int button, unsigned int mask);

    void getWindowAttrinbutes(XWindowAttributes* wa) const ;

    // atom related stuff
    std::string getTextProperity(AtomType) const ;
    std::string getTextProperity(Atom) const ;
    void setFullscreen(bool fullscreen) ;
    void dropNetActiveAtom();
    void setNetActiveAtom();
    bool getWMProtocols(Atom** protocols, int* n) const;
    //TODO: 3 of these can be unified
    void changeProperty(AtomType prop, AtomType type,
                        unsigned char* data, int data_size);
    void changeProperty(Atom prop, Atom type,
                        unsigned char* data, int data_size);

    void changeProperty(Atom prop, Atom propertyType, int format,
                        unsigned char* data, int data_size);
    bool supportsProtocol(Atom prot) const;

    // needs to be set before class usage
    static XCore* xcore;

    // get raw window
    inline Window get() const { return m_w; }

private:
    Window m_w;
};

}


