#include "wm/WindowManager.h"
#include "xlib/Xlib.h"
#include <X11/X.h>
#include <X11/Xutil.h>

WindowManager::WindowManager(xlib::XCore& x) : m_running(false), m_x(x) {
    Monitor::updateMonitors(m_monitors);
    m_monitors.focus_front();
    //TODO: update bars for all monitros;
    m_monitors.focused()->updateBar();

    // selecting wich events we want to receive for root window
    x.selectInput(
            SubstructureRedirectMask | 
            SubstructureNotifyMask |
            PropertyChangeMask
            );
}

Client* WindowManager::getClientForWindow(Window w) {
    auto it = Client::clientWindowMap.find(w);
    if (it != Client::clientWindowMap.end()) {
        return it->second;
    }
    return nullptr;
}

void WindowManager::grabKeys() {
    LOG("--Keys grabbed--");
    for (const auto& kb : config.keybindings)
        m_x.grabKey(kb.keySym, kb.mod);
}

void WindowManager::run() {
    m_running = true;

    // tell x what key events we want
    grabKeys();

    // main event loop
    while (m_running) {
		XEvent e;
		m_x.nextEvent(&e);
        switch (e.type) {
            case ConfigureRequest: onConfigureRequest(e.xconfigurerequest); break;
            case MapRequest:       onMapRequest(e.xmaprequest);             break;
            case FocusIn:          onFocusIn(e.xfocus);                     break;
            case DestroyNotify:    onDestroyNotify(e.xdestroywindow);       break;
            case MotionNotify:     onMotionNotify(e.xmotion);               break;
            case EnterNotify:      onEnterNotify(e.xcrossing);              break;
            case KeyPress:         onKeyPress(e.xkey);                      break;
            case Expose:           onExpose(e.xexpose);                     break;
            case MappingNotify:    onMappingNotify(e.xmapping);             break;
            case ClientMessage:    onClientMessage(e.xclient);              break;
            case ButtonPress:      onButtonPress(e.xbutton);                break;
            case ConfigureNotify:  onConfigureNotify(e.xconfigure);         break;
            case PropertyNotify:   onProperityNotify(e.xproperty);          break;
            case UnmapNotify:      onUnmapNotify(e.xunmap);                 break;
        }
    }
}

/* =============================== EVENT HANDLERS ========================= */

void WindowManager::onMapRequest(const XMapRequestEvent& e) {
    LOG("WM received: XMapRequestEvent");
    auto& ws = m_monitors.focused()->getSelectedWorkspace();
    // if window asking for mapping is already existing as a client
    // dont make a new client for it
    if (auto* client = getClientForWindow(e.window)) {
        return;
    }
    ws.createClientForWindow(e.window);
    ws.arrangeClients();
    ws.focusFront();
}

void WindowManager::onMotionNotify( const XMotionEvent& ) {
    LOG("WM notified: XMotionEvent ev");

    // every time the cursor moves check if the cursor went
    // to the other monitor and switch focus to that monitor
    /* 
    if (auto* nextMonitorPtr = getMonitorPtrForCoords(e.x_root, e.y_root) ){
        if (nextMonitorPtr != m_selectedMonitorRef) {
        }
    }
    */
}

void WindowManager::onProperityNotify(const XPropertyEvent& ev) {
    LOG("WM notified: ProperityEvent ev");

    // properity holding information about the wm name - used for status string
    if (ev.window == m_x.getRoot() && ev.atom == XA_WM_NAME) {
        LOG("	 -  root name / status string");
        m_monitors.focused()->getBar().setStatusString(
            m_x.getTextProperity(xlib::NetWMName));
        m_monitors.focused()->updateBar();
    }
    // properity holding information about currnet active window
    else if (ev.window == m_x.getRoot() &&
             ev.atom == m_x.getAtom(xlib::NetActiveWindow)) {
        LOG("    - net active");
        auto& m = *m_monitors.focused();

        if (m.getSelectedWorkspace().hasSelectedClient())
            m.getBar().setTitleString(
                m.getSelectedWorkspace().getSelectedClient().getTitle());
        else
            m.getBar().setTitleString("");
       
        m.updateBar();
    } else {
        LOG("	- client - ");
        if (ev.atom == XA_WM_TRANSIENT_FOR) {
            LOG("		TRANSIENT FOR - unimpl");
        } else if (ev.atom == XA_WM_NORMAL_HINTS) {
            LOG("		WM_NORMAL_HINTS - unimpl ");
        } else if (ev.atom == XA_WM_HINTS) {
            LOG("		WM_HINTS -unimpl ");
        } else if (ev.atom == XA_WM_NAME ||
                   ev.atom == m_x.getAtom(xlib::NetWMName)) {
            LOG("		WM_NAME ");
            m_monitors.focused()->getBar().setTitleString(
                xlib::XWindow { ev.window }.getTextProperity(XA_WM_NAME));
            m_monitors.focused()->updateBar();
        } else if (ev.atom == m_x.getAtom(xlib::NetWMWindowType)) {
            LOG("		WM_Window_type  -unimplemented");
        }
    }
}

void WindowManager::onMappingNotify(XMappingEvent& e) {
    LOG("WM notified: MappingEvent ev");
    m_x.refreshKeyboardMapping(e);
    if (e.request == MappingKeyboard)
        grabKeys();
}

void WindowManager::onUnmapNotify(const XUnmapEvent& e) {
    LOG("WM notified: UnmapEvent ev");
	auto* c = getClientForWindow(e.window);
	if (not c) return;
    LOG("    - Found client! unmap came from send event: " << e.send_event );

    // true if this event came from XSendEvent
    // used only for withdrawn state
    if (e.send_event) {
        c->setState(WithdrawnState);
    } else {
        auto& ws = c->getWorkspace();
        ws.removeClient(*c);
        ws.arrangeClients();
    }
}

void WindowManager::onConfigureNotify(const XConfigureEvent&) {
    LOG("WM notified: ConfigureEvent ev");
}

void WindowManager::onDestroyNotify(const XDestroyWindowEvent& e) {
    LOG("WM notified: DestoryWindow ev");
    if (auto* c = getClientForWindow(e.window)) {
        LOG("    - found client for DestroyNotify window");
        auto& ws = c->getWorkspace();
        ws.removeClient(*c); // removig a client focuses the first client
        ws.arrangeClients();
    }
}

void WindowManager::onEnterNotify(const XCrossingEvent&) {
    LOG("WM notified: CrossingEvent ev");
}

void WindowManager::onConfigureRequest(const XConfigureRequestEvent& e){
    LOG("WM received: XConfigureRequestEvent");
    if (auto* c = getClientForWindow(e.window)) {
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
        xlib::XWindow{e.window}.configureWindow(changes, e.value_mask);
    }
    m_x.sync(False);
}

void WindowManager::onKeyPress(const XKeyEvent& e){
    auto key =
        std::find_if(config.keybindings.begin(), config.keybindings.end(),
                     [&](const config::KeyBinding& kb) {
                         return m_x.keySymToKeyCode(kb.keySym) == e.keycode &&
                                static_cast<int>(e.state) == kb.mod;
                     });
    if (key != config.keybindings.end())
        key->action(key->a);
    LOG("WM received: XKeyPress Event for:");
}

void WindowManager::onButtonPress(const XButtonPressedEvent&) {
    LOG("WM received: XButtonPressedEvent");
}

void WindowManager::onClientMessage(const XClientMessageEvent& e) {
    LOG("WM received: XClienMessageEvent");
    static auto fullscreenAtom = static_cast<long>(m_x.getAtom(xlib::NetWMFullscreen));

    auto* c = getClientForWindow(e.window);
    if ( not c ) return;

    // TODO: smells
    // 2 messages aceppted
    // NetWMState -> modify fullscreen state of client
    if (e.message_type == m_x.getAtom(xlib::NetWMState)) {
        if (e.data.l[1] == fullscreenAtom || e.data.l[2] == fullscreenAtom) {
            // data.l[x] ==  1/2 netwmstateADD/toggle
            c->setFullscreen( (e.data.l[0] == 1) ||
                              (e.data.l[0] == 2  && !c->isFullscreen()));
        }
        // NerWMActiveWindow -> received when a client wants attention / is
        // urgent
    } else if (e.message_type == m_x.getAtom(xlib::NetActiveWindow)) {
        if (c != &m_monitors.focused()->getSelectedWorkspace().getSelectedClient() 
                && !c->isUrgent())
            c->setUrgent(true);
    }
}

void WindowManager::onExpose(const XExposeEvent& e) {
    LOG("WM received: XExposeEvent");
    // if no more expose events are generated
    if (e.count == 0) {
        LOG("   - bar update");
        m_monitors.focused()->updateBar();
    }
}

void WindowManager::onFocusIn(const XFocusChangeEvent& e) {
    LOG("WM received: XFocusChangeEvent");
}

/* =============================== USER ACTION HANDLERS ===================== */

void WindowManager::quit() {
    m_running = false;
}

void WindowManager::killFocused() {
    LOG("WM User triggered: kill focused");

    auto& ws = m_monitors.focused()->getSelectedWorkspace();
    if (not ws.hasSelectedClient())
        return;

    // if client doesng handle closing itself
    // WM will do it for him
    auto& c = ws.getSelectedClient();
    c.dropInputFocus();
    if (!c.sendEvent(m_x.getAtom(xlib::WMDelete))) {
        c.getXWindow().killClient();
    }
    // removing a client causes focus of front
    ws.removeClient(c);
    ws.arrangeClients();
}

void WindowManager::moveFocus(int i) {
    LOG("WM User triggered: move focus");
    auto& ws = m_monitors.focused()->getSelectedWorkspace();
    ws.moveFocus(i);
    // no need to update bar, bar is updated in onProperityChange
}

void WindowManager::moveFocusedClient(int i) {
    LOG("WM User triggered: move focused client");
    auto& ws = m_monitors.focused()->getSelectedWorkspace();
    ws.moveFocusedClient(i);
    ws.arrangeClients();
}

void WindowManager::goToWorkspace(int i) {
    LOG("Going to workspace");
    auto& m = *m_monitors.focused();
    m.selectWorkspace(i); // hides old and shows new workspace
    m.getSelectedWorkspace().arrangeClients();
    m.updateBar();
}

void WindowManager::moveFocusedClientToTop() {
    auto& ws = m_monitors.focused()->getSelectedWorkspace();
    ws.moveFocusedClientToTop();
    ws.arrangeClients();
}

void WindowManager::moveFocusedClientToWorkspace(uint i) {
    auto& ws = m_monitors.focused()->getSelectedWorkspace();
    ws.moveSelectedClientToWorkspace(
            m_monitors.focused()->getWorkspaces()[i]);
}

void WindowManager::fullscreenToggle() { 
    LOG("WM User triggered: fullscreen toggle");
    m_monitors.focused()->getSelectedWorkspace().toggleFullscreenOnSelectedClient();
    m_monitors.focused()->getSelectedWorkspace().arrangeClients();
} 

void WindowManager::resizeMaster(int i) { 
    LOG("WM User triggered: resize master");
    auto& factor = m_monitors.focused()->getSelectedWorkspace().getConfig().factor;
    if ((factor + i) > 10 && (factor + i) < 80)
        factor += i;
    m_monitors.focused()->getSelectedWorkspace().arrangeClients();
}

void WindowManager::toggleBar() {
    m_monitors.focused()->getBar().toggleHidden();
}

void WindowManager::floatToggle() { }

void WindowManager::resizeFloating() {}

void WindowManager::moveFloating() {}


