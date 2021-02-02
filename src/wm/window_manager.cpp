#include "wm/window_manager.h"

namespace wm {

window_manager::window_manager(xlib::XCore* x) : m_running(false), m_x(x) {
    // check for physical monitor configuration
    monitor::update_monitors(m_monitors);
    // select first monitor
    m_monitors.focus_front();
    // tell x serwer what events we want for root window
    m_x->selectInput(SubstructureRedirectMask | SubstructureNotifyMask |
                     PropertyChangeMask);
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
            case PropertyNotify:   on_properity_notify(e.xproperty);          break;
            case UnmapNotify:      on_unmap_notify(e.xunmap);                 break;
            // clang-format on
        }
    }
}

/* ==========================================================================
 *                              X EVENT HANDLERS
 * ========================================================================== */

/**
 * Action triggered whenever mouse pointer is moved.
 * This method checks that movement sets cursour outside
 * the current screen. If cursor is now outside the current screen
 * and we find that cursor coords are now inside the other monitor
 * we focus that monitor and that
 * @note Function is used for multimonitor purposes and nothing else
 */
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

/**
 * Action triggered when any x window changes any of its atom properities.
 * This method is consernded only with:
 *      - WM_NAME on root window
 *      - NET_ACTIVE_WINDOW on root window
 *      - WM_TRANSIENT WM_NORMAL_HINT WM_HINTS WM_NAME on any client window
 */
void window_manager::on_properity_notify(const XPropertyEvent& e) {
    LOG("WM received: XPropertyEvent");

    if (e.window == m_x->getRoot() && e.atom == XA_WM_NAME) {
        LOG("	 -  root name / status string");
        m_monitors.focused()->bar().set_status_string(
            m_x->getTextProperity(xlib::NetWMName));
        m_monitors.focused()->update_bar();
    }
    // properity holding information about currnet active window
    else if (e.window == m_x->getRoot() &&
             e.atom == m_x->getAtom(xlib::NetActiveWindow)) {
        LOG("    - net active");
        return;
        auto& m = *m_monitors.focused();
        if (m.workspaces().focused()->has_focused())
            m.bar().set_title_string(
                m.workspaces().focused()->clients().focused()->title());
        else
            m.bar().set_title_string("");
    } else {
        //TODO: impl all
        //TODO: `switch` for readability?
        LOG("	- client - ");
        if (e.atom == XA_WM_TRANSIENT_FOR) {
            LOG("		TRANSIENT FOR - unimpl");
        } else if (e.atom == XA_WM_NORMAL_HINTS) {
            LOG("		WM_NORMAL_HINTS - unimpl ");
            //TODO: nullptr check
            get_client_for_window(e.window)->update_hints();
        } else if (e.atom == XA_WM_HINTS) {
            LOG("		WM_HINTS -unimpl ");
            //TODO: nullptr check
            get_client_for_window(e.window)->update_wm_hints(false);

        } else if (e.atom == XA_WM_NAME ||
                   e.atom == m_x->getAtom(xlib::NetWMName)) {
            LOG("		WM_NAME ");
            m_monitors.focused()->bar().set_title_string(
                xlib::XWindow { e.window }.getTextProperity(XA_WM_NAME));
            m_monitors.focused()->bar();
        } else if (e.atom == m_x->getAtom(xlib::NetWMWindowType)) {
            LOG("		WM_Window_type  -unimplemented");
        }
    }
}

/**
 *
 */
void window_manager::on_mapping_notify(XMappingEvent& e) {
    LOG("WM received: XMappingEvent");
    m_x->refreshKeyboardMapping(e);
    if (e.request == MappingKeyboard) grab_keys();
}

/**
 *
 */
void window_manager::on_unmap_notify(const XUnmapEvent& e) {
    LOG("WM received: XUnmapEvent");

    auto c = get_client_for_window(e.window);
    if (not c) return;

    // used only for withdrawn state
    LOG("    - Found client! unmap came from send event: " << e.send_event);
    if (e.send_event) {
        c->set_state(WithdrawnState);
    } else {
        m_monitors.focused()->workspaces().focused()->remove_client(e.window);
    }
}

/**
 * Event used for updating changes in number of physical monitor.
 * Change in physical monitors is manifested through change in root 
 * window size. Method checks only if root was reconfigured and 
 * updates m_monitors method.
 */
void window_manager::on_configure_notify(const XConfigureEvent& e) {
    LOG("WM received: XConfigureEvent");

	if (e.window == m_x->getRoot()) {
        //TODO
	}
}

void window_manager::on_destroy_notify(const XDestroyWindowEvent& e) {
    LOG("WM received: XDestroyWindowEvent");

    if (auto* c = get_client_for_window(e.window)) {
        LOG("    - found client for DestroyNotify window");
        auto& ws = c->get_parent_workspace();
        // removig a client focuses the first client
        // no neet to call focus_fron
        ws.remove_client(c->get_xwindow().get());
    }
}

/**
 * Event trigered every time pointer enters the area of any client.
 * DWMPP checks for what client enter notify is generated and
 * gives him, his workspace & his monitor focus.
 */
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

    // TODO:
    return;

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
        //TODO:
        LOG("WM received: ConfigureRequest Event | existing client - "
            "UNIMPLEMENTED");
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

/**
 *
 */
void window_manager::on_map_request(const XMapRequestEvent& e) {
    LOG("WM received: XMapRequestEvent");
    // don't create client object for already handled window
    if (get_client_for_window(e.window)) return;
    m_monitors.focused()->workspaces().focused()->create_client(e.window);
}

/**
 *
 */
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
    static auto fullscreen_atom =
    static_cast<long>(m_x->getAtom(xlib::NetWMFullscreen));

    auto* c = get_client_for_window(e.window);
    if (not c) return;

    // TODO: smells, high priority refactor
    // 2 messages aceppted
    // NetWMState -> modify fullscreen state of client
    // if
    // data.l[x] ==  1/2 netwmstateADD/toggle
    // else
    // NerWMActiveWindow -> received when a client wants attention / is
    // urgent
#if 1
    if (e.message_type == m_x->getAtom(xlib::NetWMState)) {
        if (e.data.l[1] == fullscreen_atom || e.data.l[2] == fullscreen_atom) {
            c->set_fullscreen( 
                        (e.data.l[0] == 1) || (e.data.l[0] == 2  && !c->is_fullscreen())
                    );
        }
    } else if (e.message_type == m_x->getAtom(xlib::NetActiveWindow)) {
        auto& focused_client =
            *m_monitors.focused()->workspaces().focused()->clients().focused();
        if ((c != &focused_client) && !c->is_urgent())
            c->set_urgent(true);
    }
#endif
}

/**
 *
 */
void window_manager::on_expose(const XExposeEvent& e) {
    LOG("WM received: XExposeEvent");
    // if no more expose events are generated
    if (e.count == 0) {
        LOG("   - bar update");
        m_monitors.focused()->update_bar();
    }
}

/**
 * Focusing appropriate focus. When focus is changed checks 
 * if newly focused client is focused on 
 */
void window_manager::on_focus_in(const XFocusChangeEvent& e) {
    LOG("WM received: XFocusChangeEvent");
    auto& focused_workspace = *m_monitors.focused()->workspaces().focused();

    // TODO FIX: causes constant focus change
    return; 
    // sanity check, check if there is any focused client
    if (focused_workspace.has_focused()){
        if (focused_workspace.clients().focused()->get_xwindow().get() != e.window) {
            if(auto a = get_client_for_window(e.window)) {
                a->get_parent_workspace().set_focused_client(a);
                a->get_parent_workspace().get_parent_monitor()
                    .focus_workspace(a->get_parent_workspace().get_index());
            }
        }
    }
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

    // check if window is targeted bar
    for (auto& monitor : m_monitors) {
        if (monitor.bar().get_raw_xwindow() == w) return &monitor;
    }

    // if
    if (auto c = get_client_for_window(w))
        return &c->get_parent_workspace().get_parent_monitor();

    return &*m_monitors.focused();
}

monitor* window_manager::get_monitor_for_rectangle(const util::rect& rect) {
	auto* r = &*m_monitors.focused();
	int tmp, area = 0;

    for (auto& monitor : m_monitors) {
        if ((tmp = monitor.rect().intersection_area(rect)) > area) {
            area = tmp;
            r    = &monitor;
        }
    }
    return r;
}

monitor* window_manager::get_monitor_for_pointer_coords(int x, int y) {
    return get_monitor_for_rectangle({ { x, y }, 1, 1 });
}

}  // namespace wm
