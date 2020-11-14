#pragma once

#include "Xlib.h"

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
    XWindow(Window w);

    void moveWindow(int x, int y);
    void resizeWindow(int w, int h);
    void setWindowBorder(unsigned long pixel);
    void setWindowBorderWidth(uint w);
    void setClassHint(XClassHint ch);

    void selectInput(unsigned long mask);

    void sendEvent(XEvent& ev);
    void sendEvent(XEvent& ev, int eventMask);
    void configureWindow(XWindowChanges& changes, unsigned int mask);

    void setInputFocus();
    void raiseWindow();
    void mapWindow();
    void mapRaised();
    void killClient();

    void grabButton(unsigned int button, unsigned int mask);

    void getWindowAttrinbutes(XWindowAttributes* wa);

    // atom related stuff
    std::string getTextProperity(AtomType);
    std::string getTextProperity(Atom);
    void setFullscreen(bool fullscreen) ;
    void setActive();
    bool getWMProtocols(Atom** protocols, int* n);
    void changeProperty(Atom prop, AtomType type,
                        unsigned char* data, int data_size);
    bool supportsProtocol(Atom prot);

    // needs to be set before class usage
    static XCore* xcore;

    // get raw window
    inline Window get() const { return m_w; }

private:
    Window m_w;
};

}


