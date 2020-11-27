#include "wm/client.h"
#include "xlib/XCore.h"
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
    //m_oldTopLeft     = { wa.x, wa.y };
    //m_oldDimensions  = { wa.width, wa.height };
    //m_oldBorderWidth = wa.border_width;
}

client::~client() {
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

void client::move_resize(const util::area& area) {
    m_position = area.top_left;
    m_size = util::point { (int)area.width, (int)area.height };
    m_xwindow.moveWindow(area.top_left.x, area.top_left.y);
    m_xwindow.resizeWindow(area.width, area.height);
}

std::string client::title() const {
    return m_xwindow.getTextProperity(xlib::NetWMName);
}

void client::take_input_focus() { 
    m_xwindow.setInputFocus();
    // set atom properity of active window to be this clients window
    m_xwindow.setActive();
    m_xwindow.setWindowBorder(conf.focused_border.get().pixel);
    // no neet to trySendEvent, everything should support WMTakeFocus
    m_xwindow.sendEvent(xlib::WMTakeFocus); 
}

void client::drop_input_focus() { 
    // TODO: smells..
    XSetInputFocus(m_xwindow.xcore->getDpyPtr(), m_xwindow.xcore->getRoot(),
                   RevertToPointerRoot, CurrentTime);
    m_xwindow.setWindowBorder(conf.unfocused_border.get().pixel);
}

void client::set_state(long state) {
    long data[] = { state, None };
    m_xwindow.changeProperty(xlib::WMState, xlib::WMState,
                             reinterpret_cast<unsigned char*>(data), 2);
}

void client::kill() {
    if (not m_xwindow.trySendEvent(xlib::WMDelete)) m_xwindow.killClient();
}

void client::toggle_floating() {
    m_floating = not m_floating;
}

void client::raise() { m_xwindow.raiseWindow(); }
void client::hide() { m_xwindow.moveWindow(-m_size.x - m_position.x - 1, 0); }
void client::show() { m_xwindow.moveWindow(m_position.x, m_position.y); }
void client::set_parent_workspace(workspace* ws) { m_parent_workspace = ws; }
void client::set_fullscreen(bool) {}

}  // namespace wm
