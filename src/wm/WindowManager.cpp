#include "wm/WindowManager.h"
#include "xlib/Xlib.h"
#include <X11/X.h>

WindowManager::WindowManager(xlib::XCore& x) : m_running(false), m_x(x) {
    Monitor::updateMonitors(m_monitors);
    m_monitors.focus_front();

    // selecting wich events we want to reive
    x.selectInput(
            SubstructureRedirectMask | 
            SubstructureNotifyMask |
            FocusChangeMask |          // onFocusIn
            PropertyChangeMask);
}

Client* WindowManager::getClientForWindow(Window w) {
    for (auto& m : m_monitors)
        for (auto& ws : m.getWorkspaces())
            for (auto& c: ws.getClients())
                if (c.getXWindow().get() == w)
                    return &c;
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
    m_monitors.focused()->getBar().update();
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
    if( ev.window == m_x.getRoot() && ev.atom == XA_WM_NAME ){
		LOG("	 root name / status string" );
		m_monitors.focused()->updateBar();
	}
    else if (ev.atom == m_x.getAtom(xlib::NetActiveWindow)) {
		LOG("    net active" );
        auto& m = *m_monitors.focused();
        if (m.getSelectedWorkspace().hasSelectedClient())
            m.getBar().setTitleString(
                m.getSelectedWorkspace().getSelectedClient().getTitle());
        // in client class active atom is set but never cleared
        m.updateBar();
    }
}

void WindowManager::onMappingNotify(XMappingEvent& e) {
    LOG("WM notified: MappingEvent ev");
    m_x.refreshKeyboardMapping(e);
    if (e.request == MappingKeyboard)
        grabKeys();
}

void WindowManager::onUnmapNotify( const XUnmapEvent& ){
    LOG("WM notified: UnmapEvent ev");
}

void WindowManager::onConfigureNotify(const XConfigureEvent& ) {
    LOG("WM notified: ConfigureEvent ev");
}

void WindowManager::onDestroyNotify(const XDestroyWindowEvent& e) {
    LOG("WM notified: DestoryWindow ev");
    if (auto* c = getClientForWindow(e.window)) {
        LOG("\tfound client for DestroyNotify window");
        auto& ws = c->getWorkspace();
        ws.removeClient(*c); // removig a client focuses the first client
        ws.arrangeClients();
    }
}

void WindowManager::onEnterNotify(const XCrossingEvent& ) {
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
    m_x.sync( False );
}

void WindowManager::onKeyPress(const XKeyEvent& e){
    auto key = std::find_if(config.keybindings.begin(),
                     config.keybindings.end(),
                     [&](const config::KeyBinding& kb) {
                         return m_x.keySymToKeyCode(kb.keySym) == e.keycode &&
                                static_cast<int>(e.state) == kb.mod;
                     });
    if (key != config.keybindings.end())
        key->action(key->a);
    LOG("WM received: XKeyPress Event for:");
}

void WindowManager::onButtonPress(const XButtonPressedEvent& e) {
    LOG("WM received: XButtonPressedEvent");
}

void WindowManager::onClientMessage(const XClientMessageEvent& e) {
    LOG("WM received: XClienMessageEvent");
}

void WindowManager::onExpose(const XExposeEvent& e) {
    LOG("WM received: XExposeEvent");
    // if no more expose events are generated
    if (e.count == 0 ) {
        LOG(" bar update");
    }
}

void WindowManager::onFocusIn(const XFocusChangeEvent& e) {
    auto& m = *m_monitors.focused();
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
    // bar is updaate in focusin event
}

void WindowManager::moveFocusedClient(int i) {
    LOG("WM User triggered: move focused client");
    auto& ws = m_monitors.focused()->getSelectedWorkspace();
    ws.moveFocusedClient(i);
    ws.arrangeClients();
}

void WindowManager::goToWorkspace(int i) {
    LOG("Going to workspace");
    //TODO: consider holding this inside Monitor::selectWorkspace
    m_monitors.focused()->getSelectedWorkspace().hideAllClients();
    m_monitors.focused()->selectWorkspace(i);
    m_monitors.focused()->getSelectedWorkspace().showAllClients();
    m_monitors.focused()->getSelectedWorkspace().arrangeClients();
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

void WindowManager::fullscreenToggle(){}

void WindowManager::floatToggle(){}

void WindowManager::resizeMaster(int) { }

void WindowManager::resizeFloating(){}

void WindowManager::moveFloating(){}

void WindowManager::toggleBar(){ }

