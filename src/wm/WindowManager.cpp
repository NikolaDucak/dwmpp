#include "wm/WindowManager.h"

void WindowManager::grabKeys() {
    LOG("--Keys grabbed--");
    for (const auto& kb : config::windowManager.keybindings)
        m_x.grabKey(kb.keySym, kb.mod);
}

WindowManager::WindowManager(xlib::XCore& x) : m_running(false), m_x(x) {
    Monitor::updateMonitors(m_monitors);
    m_focusedMonitorPtr = &m_monitors.front();

    x.selectInput(SubstructureRedirectMask | SubstructureNotifyMask |
                  PropertyChangeMask);
}

void WindowManager::run(){
    m_running = true;

    // tell x what key events we want
    grabKeys();

    while (m_running) {
		XEvent e;
		m_x.nextEvent(&e);
        LOG("EV");
        switch (e.type) {
            case ConfigureRequest: onConfigureRequest(e.xconfigurerequest); break;
            case MapRequest: onMapRequest(e.xmaprequest); break;
            case FocusIn: onFocusIn(e.xfocus); break;
            case DestroyNotify: onDestroyNotify(e.xdestroywindow); break;
            case MotionNotify: onMotionNotify(e.xmotion); break;
            case EnterNotify: onEnterNotify(e.xcrossing); break;
            case KeyPress: onKeyPress(e.xkey); break;
            case Expose: onExpose(e.xexpose); break;
            case MappingNotify: onMappingNotify(e.xmapping); break;
            case ClientMessage: onClientMessage(e.xclient); break;
            case ButtonPress: onButtonPress(e.xbutton); break;
            case ConfigureNotify: onConfigureNotify(e.xconfigure); break;
            case PropertyNotify: onProperityNotify(e.xproperty); break;
            case UnmapNotify: onUnmapNotify(e.xunmap); break;
        }
    }
}

/* =============================== EVENT HANDLERS ========================= */

void WindowManager::onMapRequest(const XMapRequestEvent& e) {
    LOG("WM received: XMapRequestEvent");
    // if window asking for mapping is already existing as a client
    // dont make a new client for it
    
    // workspace will automaticaly set focus for that client and 
    // add stuff
    m_focusedMonitorPtr->getSelectedWorkspace().createClientForWindow(e.window);
    m_focusedMonitorPtr->getSelectedWorkspace().arrangeClients(
            10, m_focusedMonitorPtr->getSize());
    m_focusedMonitorPtr->getSelectedWorkspace().focusFront();
}

void WindowManager::onMotionNotify( const XMotionEvent& e ) {
    LOG("WM notified: XMotionEvent ev");

    // every time the cursor moves check if the cursor went
    // to the other monitor and switch focus to that monitor
    /* 
    if (auto nextMonitorPtr = getMonitorPtrForCoords(e.x_root, e.y_root) ){
        if (nextMonitorPtr != m_selectedMonitorRef) {
        }
    }
    */
}

void WindowManager::onProperityNotify(const XPropertyEvent& e) {
    LOG("WM notified: ProperityEvent ev");
}

void WindowManager::onMappingNotify(XMappingEvent& e) {
    LOG("WM notified: MappingEvent ev");
    m_x.refreshKeyboardMapping(e);
    if (e.request == MappingKeyboard)
        grabKeys();
}

void WindowManager::onUnmapNotify( const XUnmapEvent& e){
    LOG("WM notified: UnmapEvent ev");
}

void WindowManager::onConfigureNotify(const XConfigureEvent& e) {
    LOG("WM notified: ConfigureEvent ev");
}

void WindowManager::onDestroyNotify(const XDestroyWindowEvent& e) {
    LOG("WM notified: DestoryWindow ev");
}

void WindowManager::onEnterNotify(const XCrossingEvent& e) {
    LOG("WM notified: CrossingEvent ev");
}

void WindowManager::onConfigureRequest(const XConfigureRequestEvent& e){
    LOG("WM received: XConfigureRequestEvent");
}

void WindowManager::onKeyPress(const XKeyEvent& e){
	auto key = std::find_if( config::windowManager.keybindings.begin(), config::windowManager.keybindings.end(), 
		[&]( const config::KeyBinding& kb ){
			return m_x.keySymToKeyCode( kb.keySym ) == e.keycode && 
				   static_cast<int>(e.state) == kb.mod;
		});
	if( key != config::windowManager.keybindings.end() )
		key->action( key->a );
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
}

void WindowManager::onFocusIn(const XFocusChangeEvent& e) {
    LOG("WM received: XFocusChangeEvent");
}

void WindowManager::quit(){
    m_running = false;
}

void WindowManager::killFocused() {
    LOG("WM User triggered: kill focused");
    auto& ws = m_focusedMonitorPtr->getSelectedWorkspace();
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
    ws.arrangeClients( 10, ws.getMonitor().getSize() );
}

void WindowManager::moveFocus(int i){
    LOG("WM User triggered: move focus");
    auto& ws = m_focusedMonitorPtr->getSelectedWorkspace();
    ws.moveFocus(i);
}

void WindowManager::moveFocusedClient(int i){
    LOG("WM User triggered: move focused client");
    auto& ws = m_focusedMonitorPtr->getSelectedWorkspace();
    ws.moveFocusedClient(i);
    ws.arrangeClients(10,ws.getMonitor().getSize());
}

void WindowManager::goToWorkspace(int i){}


void WindowManager::moveFocusedClientToTop() {
}
void WindowManager::moveFocusedClientToWorkspace(uint i){}
void WindowManager::fullscreenToggle(){}
void WindowManager::floatToggle(){}
void WindowManager::resizeMaster(int i){}
void WindowManager::resizeFloating(){}
void WindowManager::moveFloating(){}
void WindowManager::toggleBar(){}

void WindowManager::configure(const config::WMConfig&) {

}

