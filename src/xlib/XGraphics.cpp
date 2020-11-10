#include "xlib/XGraphics.h"

namespace xlib {

XCore* XGraphics::xcore;

XGraphics::XGraphics() {
    auto dpy_ = xcore->getDpyPtr();
    gc_       = XCreateGC(dpy_, xcore->getRoot(), 0, NULL);
    drawable_ = XCreatePixmap(dpy_, xcore->getRoot(),
                              DisplayWidth(xcore->getDpyPtr(),  xcore->getScreen()),
                              DisplayHeight(xcore->getDpyPtr(), xcore->getScreen()),
                              DefaultDepth(xcore->getDpyPtr(),  xcore->getScreen()));
    XSetLineAttributes(dpy_, gc_, 1, LineSolid, CapButt, JoinMiter);
}

void XGraphics::drawText(XWindow& w, const XFont& fnt, const XColor& clr, 
              point xy, const std::string& text) {
    Display* dpy_  = xcore->getDpyPtr();
    int scr       = DefaultScreen(dpy_);
    int ty        = xy.y + fnt.get()->ascent;  // correct text position
    XftDraw* draw = XftDrawCreate(dpy_, w.get(), DefaultVisual(dpy_, scr),
                                  DefaultColormap(dpy_, scr));
    XftDrawStringUtf8(draw, &clr.get(), const_cast<XftFont*>(fnt.get()), xy.x, ty,
                      (const FcChar8*)text.c_str(), text.size());
    XftDrawDestroy(draw);
}

void XGraphics::fillRectangle(const xlib::XColor& c, point xy, point wh) {
    Display* dpy_  = xcore->getDpyPtr();
    XSetForeground(dpy_, gc_, c.get().pixel);
    XFillRectangle(dpy_, drawable_, gc_, xy.x, xy.y, wh.x, wh.y);
}

void XGraphics::copyArea(Window w, point xy, point wh) {
    Display* dpy_  = xcore->getDpyPtr();
    XCopyArea(dpy_, drawable_, w, gc_, xy.x, xy.y, wh.x, wh.y, xy.x, xy.y);
    XSync(dpy_, False);
}

}