#include "wm/client.h"
#include "xlib/XCore.h"
#include <X11/X.h>
#include <unordered_map>

namespace wm {

std::unordered_map<Window, client*> client::window_client_map;

client::client(Window w, workspace* parent_workspace) :
    m_floating(false),
    m_xwindow(w),
    m_parent_workspace(parent_workspace) {
    // select events for that window
    m_xwindow.selectInput(EnterWindowMask | FocusChangeMask |
                          PropertyChangeMask | StructureNotifyMask);

    m_xwindow.setWindowBorderWidth(conf.border_width);
    // asing pointer to client object for this window
    window_client_map[m_xwindow.get()] = this;
    
    XWindowAttributes wa;
    m_xwindow.getWindowAttrinbutes(&wa);
    m_position = { wa.x, wa.y };
    m_size     = { wa.width, wa.height };
}

client::~client() {
    // deleting this object marks Window as unhandled, or
    // if Window is deleted then this obj has no reason to live
    window_client_map.erase(m_xwindow.get());
}

void client::move(const util::point& where) {
    m_position = where;
    m_xwindow.moveWindow(where.x, where.y);
}

void client::resize(const util::point& size) {
    m_size = size;
    m_xwindow.resizeWindow(size.x,size.y);
}

void client::move_resize(const util::rect& area) {
    m_position = area.top_left;
    m_size = util::point { (int)area.width, (int)area.height };
    m_xwindow.moveWindow(area.top_left.x, area.top_left.y);
    m_xwindow.resizeWindow(area.width, area.height);
}

std::string client::title() const {
    // "Title" (text that usualy sits in the top midle frame of the window) is
    // stored in NetWMName atom value
    return m_xwindow.getTextProperity(xlib::NetWMName);
}

void client::take_input_focus() { 
    m_xwindow.setInputFocus();
    // set atom properity of active window to be this clients window
    m_xwindow.setNetActiveAtom();
    m_xwindow.setWindowBorder(conf.focused_border.get().pixel);
    // no neet to trySendEvent, everything should support WMTakeFocus
    m_xwindow.sendEvent(xlib::WMTakeFocus); 
}

void client::drop_input_focus() { 
    // TODO: move to xlib
    //m_xwindow.dropInputFocus()
    XSetInputFocus(m_xwindow.xcore->getDpyPtr(), m_xwindow.xcore->getRoot(),
                   RevertToPointerRoot, CurrentTime);
    m_xwindow.dropNetActiveAtom();
    m_xwindow.setWindowBorder(conf.unfocused_border.get().pixel);
}

void client::set_state(long state) {
    long data[] = { state, None };
    m_xwindow.changeProperty(xlib::WMState, xlib::WMState,
                             reinterpret_cast<unsigned char*>(data), 2);
}
void client::kill() {
    // remove as active, forcing bar update
    m_xwindow.dropNetActiveAtom(); 
    if (not m_xwindow.trySendEvent(xlib::WMDelete)) {
        m_xwindow.killClient();
    }
}

std::optional<Window> client::get_transient_for() {
    return m_xwindow.getTransientFor();
}


void client::update_hints() {
    auto dpy = m_xwindow.xcore->getDpyPtr();
    long       msize;
    XSizeHints size;

    if (!XGetWMNormalHints(dpy, m_xwindow.get(), &size, &msize))
        /* size is uninitialized, ensure that size.flags aren't used */
        size.flags = PSize;
    if (size.flags & PBaseSize) {
        m_size_hints.base_w = size.base_width;
        m_size_hints.base_h = size.base_height;
    } else if (size.flags & PMinSize) {
        m_size_hints.base_w = size.min_width;
        m_size_hints.base_h = size.min_height;
    } else
        m_size_hints.base_w = m_size_hints.base_h = 0;
    if (size.flags & PResizeInc) {
        m_size_hints.inc_w = size.width_inc;
        m_size_hints.inc_h = size.height_inc;
    } else
        m_size_hints.inc_w = m_size_hints.inc_h = 0;
    if (size.flags & PMaxSize) {
        m_size_hints.max_w = size.max_width;
        m_size_hints.max_h = size.max_height;
    } else
        m_size_hints.max_w = m_size_hints.max_h = 0;
    if (size.flags & PMinSize) {
        m_size_hints.min_w = size.min_width;
        m_size_hints.min_h = size.min_height;
    } else if (size.flags & PBaseSize) {
        m_size_hints.min_w = size.base_width;
        m_size_hints.min_h = size.base_height;
    } else
        m_size_hints.min_w = m_size_hints.min_h = 0;
    if (size.flags & PAspect) {
        m_size_hints.min_a = (float)size.min_aspect.y / size.min_aspect.x;
        m_size_hints.max_a = (float)size.max_aspect.x / size.max_aspect.y;
    } else
        m_size_hints.max_a = m_size_hints.min_a = 0.0;
    m_size_hints.is_fixed = (m_size_hints.max_w && m_size_hints.max_h &&
                             m_size_hints.max_w == m_size_hints.min_w &&
                             m_size_hints.max_h == m_size_hints.min_h);
}

void client::update_wm_hints(bool is_focused) {
    //TODO: move to xlib::XWindow;
    auto dpy = m_xwindow.xcore->getDpyPtr();
    XWMHints* wmh;

    if ((wmh = XGetWMHints(dpy, m_xwindow.get()))) {
        if (is_focused && wmh->flags & XUrgencyHint) {
            wmh->flags &= ~XUrgencyHint;
            XSetWMHints(dpy, m_xwindow.get(), wmh);
        } else
            m_urgent = (wmh->flags & XUrgencyHint) ? 1 : 0;
        if (wmh->flags & InputHint)
            m_never_focus = !wmh->input;
        else
            m_never_focus = 0;
        XFree(wmh);
    }
}

void client::update_window_type() {
    auto state = m_xwindow.getAtomProperty(xlib::NetWMState);
    auto wtype = m_xwindow.getAtomProperty(xlib::NetWMWindowType);

    if (state == m_xwindow.xcore->getAtom(xlib::NetWMFullscreen))
        set_fullscreen(true);
    if (wtype == m_xwindow.xcore->getAtom(xlib::NetWMWindowTypeDialog))
        m_floating = true;
}

void client::toggle_floating() { m_floating = not m_floating; }
void client::raise() { m_xwindow.raiseWindow(); }
void client::hide() { m_xwindow.moveWindow(-m_size.x - m_position.x - 1, 0); }
void client::show() { m_xwindow.moveWindow(m_position.x, m_position.y); }
void client::set_parent_workspace(workspace* ws) { m_parent_workspace = ws; }

}  // namespace wm
