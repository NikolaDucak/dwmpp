#include "wm/window_manager.h"
#include "xlib/XCore.h"
#include "xlib/XWindow.h"
#include <X11/Xlib.h>

namespace wm {

window_manager::window_manager(xlib::XCore* x) : m_running(false), m_x(x) {
    // check for physical monitor configuration
    monitor::update_monitors(m_monitors);
    // focusing revalidates internal focus iterator in case of updating monitors
    m_monitors.focus_front();
    // tell x serwer what events we want for root window
    m_x->selectInput(SubstructureRedirectMask | SubstructureNotifyMask |
                     PropertyChangeMask);
}

window_manager::~window_manager() {
    // send kill events to all clients 
    // TODO: reconsider workspace or client managing client killing or
    // maybe on client destructor if client obj lifetime matches xorg client lifetime?
    for (auto& monitor : m_monitors)
        for (auto& workspace : monitor.workspaces())
            for (auto& client : workspace.clients())
                client.kill();

    // TODO: move to xlib
    XCloseDisplay(m_x->getDpyPtr());
}

void window_manager::run() {
    m_running = true;
    // tell x what key events we want
    grab_keys();

    // main event loop
    while (m_running) {
        XEvent e;
        m_x->nextEvent(&e);
        switch (e.type) {
            // clang-format off
            case ConfigureRequest: on_configure_request(e.xconfigurerequest); break;
            case MapRequest:       on_map_request(e.xmaprequest);             break;
            case FocusIn:          on_focus_in(e.xfocus);                     break;
            case DestroyNotify:    on_destroy_notify(e.xdestroywindow);       break;
            case MotionNotify:     on_motion_notify(e.xmotion);               break;
            case EnterNotify:      on_enter_notify(e.xcrossing);              break;
            case KeyPress:         on_key_press(e.xkey);                      break;
            case Expose:           on_expose(e.xexpose);                      break;
            case MappingNotify:    on_mapping_notify(e.xmapping);             break;
            case ClientMessage:    on_client_message(e.xclient);              break;
            case ButtonPress:      on_button_press(e.xbutton);                break;
            case ConfigureNotify:  on_configure_notify(e.xconfigure);         break;
            case PropertyNotify:   on_property_notify(e.xproperty);          break;
            case UnmapNotify:      on_unmap_notify(e.xunmap);                 break;
            // clang-format on
        }
    }
}

/* ==========================================================================
 *                              X EVENT HANDLERS
 * ========================================================================== */

void window_manager::on_motion_notify(const XMotionEvent& e) {
    LOG("WM received: XMotionEvent");
    static monitor* old_monitor = NULL;

    // proces motion only when pointer is pointing on root window
    // (not on any client window)
    if (e.window != m_x->getRoot()) return;
    auto new_monitor = get_monitor_for_pointer_coords(e.x_root, e.y_root);

    if ((new_monitor != old_monitor) && old_monitor) focus_monitor(new_monitor);
    old_monitor = new_monitor;
}

void window_manager::on_property_notify(const XPropertyEvent& e) {
    LOG("WM received: XPropertyEvent");
    static const auto root = m_x->getRoot();

    // status string is stored as name of root window
    if (e.window == root && e.atom == XA_WM_NAME) {
        LOG("	 -  root name / status string");
        m_monitors.focused()->bar().set_status_string(
            m_x->getTextProperity(XA_WM_NAME));
        m_monitors.focused()->update_bar();
    }
    // properity holding information about currnet active window
    else if (e.window == root &&
             e.atom == m_x->getAtom(xlib::NetActiveWindow)) {
        LOG("    - net active");
        Window a = m_x->readActiveWindowProperty(); // TODO: myb switch to std::optional
        if (a != 0) 
            m_monitors.focused()->bar().set_title_string(
                xlib::XWindow { a }.getTextProperity(XA_WM_NAME));
         else 
            m_monitors.focused()->bar().set_title_string("");
        
        m_monitors.focused()->update_bar();
    } else {
        auto c = get_client_for_window(e.window);
        switch (e.atom) {
            case XA_WM_TRANSIENT_FOR: { // popup windows
                LOG("		TRANSIENT FOR");
                if (!c->is_floating()) {
                    if (auto t = c->get_transient_for() ) {
                        auto f = get_client_for_window(t.value()) != nullptr;
                        c->set_floating(f);
                        if (f) c->get_parent_workspace().arrange();
                    }
                }
                break;
            }
            case XA_WM_NORMAL_HINTS:
                LOG("		WM_NORMAL_HINTS");
                c->update_hints();
                break;
            case XA_WM_HINTS:
                LOG("		WM_HINTS");
                c->update_wm_hints(false);
                break;
            case XA_WM_NAME:
                LOG("		XA_WM_NAME ");
                if (c != &get_focused_client())
                    return;
                m_monitors.focused()->bar().set_title_string(xlib::XWindow { e.window }.getTextProperity(XA_WM_NAME));
                m_monitors.focused()->update_bar();
                break;
        }
        // non constexpr... cant fit in switch
        if (e.atom == m_x->getAtom(xlib::NetWMWindowType)) {
            LOG("		WM_Window_type");
            c->update_wm_hints(c != &get_focused_client());
        }
        if (e.atom == m_x->getAtom(xlib::NetWMName)) {
            if (c != &get_focused_client())
                return;
            LOG("		NET_WM_NAME ");
            m_monitors.focused()->bar().set_title_string( xlib::XWindow { e.window }.getTextProperity(XA_WM_NAME));
            m_monitors.focused()->bar();
        }
    }
}

void window_manager::on_mapping_notify(XMappingEvent& e) {
    LOG("WM received: XMappingEvent");
    m_x->refreshKeyboardMapping(e);
    if (e.request == MappingKeyboard) grab_keys();
}

void window_manager::on_unmap_notify(const XUnmapEvent& e) {
    LOG("WM received: XUnmapEvent");

    auto c = get_client_for_window(e.window);
    if (not c) return;

    // used only for withdrawn state
    LOG("    - Found client! unmap came from send event: " << e.send_event);
    if (e.send_event) {
        c->set_state(WithdrawnState);
    } else {
        c->get_parent_workspace().remove_client(e.window);
    }
}

void window_manager::on_configure_notify(const XConfigureEvent& e) {
    LOG("WM received: XConfigureEvent");

	if (e.window == m_x->getRoot()) {
        // TODO: in order to finish handling this event, multi monitor
        // support needs to be added
	}
}

void window_manager::on_destroy_notify(const XDestroyWindowEvent& e) {
    LOG("WM received: XDestroyWindowEvent");
    // if destroyed window is not mapped by dwmpp no
    // need to do anything
    if (auto* c = get_client_for_window(e.window)) {
        LOG("    - found client for DestroyNotify window");
        auto& ws = c->get_parent_workspace();
        // removig a client focuses the first client
        // no need to call focus_front
        ws.remove_client(c->get_xwindow().get());
    }
}

void window_manager::on_enter_notify(const XCrossingEvent& e) {
    LOG("WM received: XCrossingEvent");

    if ((e.mode != NotifyNormal || e.detail == NotifyInferior) &&
        e.window != m_x->getRoot())
        return;

    auto* c = get_client_for_window(e.window);

    // get monitor for client
    auto* m = c ? &c->get_parent_workspace().get_parent_monitor() :
                  get_monitor_for_window(e.window);

    // if focused client is entered client, no action
    if (&*m->workspaces().focused()->clients().focused() == c) return;

    // else unfocus current selected client
    m_monitors.focused()->workspaces().focused()->unfocus();

    // select new monitor if needed
    if (m != &*m_monitors.focused()) { focus_monitor(m); }
    
    // set selected client iterator in workspace
    m_monitors.focused()->workspaces().focused()->set_focused_client(c);
}

void window_manager::on_configure_request(const XConfigureRequestEvent& e) {
    LOG("WM received: XConfigureRequestEvent");

    if (auto* c = get_client_for_window(e.window)) {
        LOG("WM received: ConfigureRequest Event | existing client - ");
        // configuring position & size of clien is alowed only on floating clients
        if (not c->is_floating()) return; //TODO: dwm call configure(c);
        //TODO: impl
    } else {
        LOG("WM received: ConfigureRequest Event | new client");
        XWindowChanges changes {
            .x            = e.x,
            .y            = e.y,
            .width        = e.width,
            .height       = e.height,
            .border_width = e.border_width,
            .sibling      = e.above,
            .stack_mode   = e.detail,
        };
        xlib::XWindow { e.window }.configureWindow(changes, e.value_mask);
    }
    m_x->sync(False);
}

void window_manager::on_map_request(const XMapRequestEvent& e) {
    LOG("WM received: XMapRequestEvent");
    // don't create client object for already handled window
    if (get_client_for_window(e.window)) return;
    // let the current workspace handle client creation and arrangement
    m_monitors.focused()->workspaces().focused()->create_client(e.window);
}

void window_manager::on_key_press(const XKeyEvent& e) {
    LOG("WM received: XKeyEvent");
    auto key =
        std::find_if(conf.keybindings.begin(), conf.keybindings.end(),
                     [&](const keybinding& kb) {
                         return m_x->keySymToKeyCode(kb.keySym) == e.keycode &&
                                static_cast<int>(e.state) == kb.mod;
                     });

    // call key action
    if (key != conf.keybindings.end()) { key->action->execute(*this); }
}

void window_manager::on_button_press(const XButtonPressedEvent&) {
    LOG("WM received: XButtonPressedEvent");
    // TODO: implementaiton
}

void window_manager::on_client_message(const XClientMessageEvent& e) {
    LOG("WM received: XClientMessageEvent");
    static auto wm_state_atom = m_x->getAtom(xlib::NetWMState);
    static auto fullscreen_atom =
        static_cast<long>(m_x->getAtom(xlib::NetWMFullscreen));
    auto* c = get_client_for_window(e.window);

    if (not c) return;

    if (e.message_type == wm_state_atom) {
        if (e.data.l[1] == fullscreen_atom || e.data.l[2] == fullscreen_atom) {
            c->set_fullscreen((e.data.l[0] == 1) ||
                              (e.data.l[0] == 2 && !c->is_fullscreen()));
        }
    } else if (e.message_type == m_x->getAtom(xlib::NetActiveWindow)) {
        auto& focused_client = get_focused_client();
        if ((c != &focused_client) && !c->is_urgent()) c->set_urgent(true);
    }
}

void window_manager::on_expose(const XExposeEvent& e) {
    LOG("WM received: XExposeEvent");
    // if no more expose events are generated
    if (e.count == 0) {
        LOG("   - bar update");
        m_monitors.focused()->update_bar();
    }
}

void window_manager::on_focus_in(const XFocusChangeEvent& ) {
    // LOG("WM received: XFocusChangeEvent");
    // TODO: is this event necessary?
}

/* ==========================================================================
 *                              UTILITIES
 * ========================================================================== */

void window_manager::grab_keys() {
    for (const auto& kb : conf.keybindings)
        m_x->grabKey(kb.keySym, kb.mod);
}

void window_manager::focus_monitor(monitor* m) {
    // remove bar title, status & drop focus from selected client
    m_monitors.focused()->unfocus();
    // circulate monitor list until desired monitor is marked as focused
    m_monitors.focus_front();
    while (&*m_monitors.focused() != m)
        m_monitors.circulate_focus(1);

    // set bar title & give focus to selected client
    m_monitors.focused()->focus();
}

client* window_manager::get_client_for_window(Window w) {
    auto it = client::window_client_map.find(w);

    if (it != client::window_client_map.end())
        return it->second;
    else
        return nullptr;
}

monitor* window_manager::get_monitor_for_window(Window w) {
    int x, y;

    // if window is root, since root whole multi-screen area
    // get pointer cooridinates relative to root & check
    // to which monitor they belong
    if (w == m_x->getRoot() && m_x->queryPointer(&x, &y))
        return get_monitor_for_pointer_coords(x, y);

    // check if desired window is bar on some monitor
    for (auto& monitor : m_monitors)
        if (monitor.bar().get_raw_xwindow() == w) return &monitor;

    // else check if window is handled by wm::client and get his parent monitor
    if (auto c = get_client_for_window(w))
        return &c->get_parent_workspace().get_parent_monitor();
    
    // if everything fails, assume window is on current monitor
    return &*m_monitors.focused();
}

client& window_manager::get_focused_client() {
    return *m_monitors.focused()->workspaces().focused()->clients().focused();
}

monitor* window_manager::get_monitor_for_rectangle(const util::rect& rect) {
	auto* desired_monitor = &*m_monitors.focused();
	int tmp_area, area = 0;

    for (auto& monitor : m_monitors) {
        // find wich monitor has the largest intersection area with rectange
        if ((tmp_area = monitor.rect().intersection_area(rect)) > area) {
            area            = tmp_area;
            desired_monitor = &monitor;
        }
    }

    return desired_monitor;
}

monitor* window_manager::get_monitor_for_pointer_coords(int x, int y) {
    return get_monitor_for_rectangle({ { x, y }, 1, 1 });
}

}  // namespace wm
