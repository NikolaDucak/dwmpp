#include "WM.h"

/* ================================================================================
 *									ATOMS
 * =============================================================================== */

void WM::updateClientList() {
	xatoms_.deleteClientList();
	for( auto& m : monitors_ )
		for( auto& w : m.getWorkspaces() )
			for( auto& c: w.getClients() )
				xatoms_.appendToClientList( c.getXWindow() );
}


/* ================================================================================
 *									SINGLETON
 * =============================================================================== */

WM& WM::instance() {
	static WM wm( XLib::instance() );
	return wm;
}

WM::WM( XLib& xcore ) : 
	running_( true ),
	x_( xcore ),
	xatoms_( x_ ),
	monitors_()
{
	Monitor::updateMonitors( monitors_ );	
	selectedMonitor_ = &monitors_.front();
	x_.selectInput( 
			SubstructureRedirectMask | 
			SubstructureNotifyMask | 
			PropertyChangeMask
	);
}

void WM::run() {
	grabKeys();
	selectedMonitor_->updateBar();
	while( running_ ){
		XEvent e;
		x_.nextEvent( e );
		switch( e.type ){
			// Requests
			case ConfigureRequest: onConfigureRequest( e.xconfigurerequest ); break;
			case MapRequest:       onMapRequest( e.xmaprequest );             break;
			case FocusIn:          onFocusIn( e.xfocus );                     break;
			// Notifications
			case DestroyNotify:    onDestroyNotify( e.xdestroywindow );       break;
			case MotionNotify:     onMotionNotify( e.xmotion );               break;
			case EnterNotify:      onEnterNotify( e.xcrossing );              break;
			// input
			case KeyPress:         onKeyPress( e.xkey );                      break;
			// other
			case Expose:           onExpose( e.xexpose );                     break;
			case MappingNotify:    onMappingNotify( e.xmapping );             break;
			case ClientMessage:    onClientMessage( e.xclient );              break;

			// WIP:input
			case ButtonPress:      onButtonPress( e.xbutton );                break;
			// WIM: notify
			case ConfigureNotify:  onConfigureNotify( e.xconfigure );         break;
			case PropertyNotify:   onProperityNotify( e.xproperty );          break;
			case UnmapNotify:      onUnmapNotify( e.xunmap );                 break;
			// wip events
		}
	}
}

#define MAX(A, B)               ((A) > (B) ? (A) : (B))
#define MIN(A, B)               ((A) < (B) ? (A) : (B))
#define BETWEEN(X, A, B)        ((A) <= (X) && (X) <= (B))
#define INTERSECT(tx,ty,tw,th,m)    (MAX(0, MIN((tx)+(tw),(m)->getWinPos().x+(m)->getWinSize().x) - MAX((tx),(m)->getWinPos().x)) \
                                   * MAX(0, MIN((ty)+(th),(m)->getWinPos().y+(m)->getWinSize().y) - MAX((ty),(m)->getWinPos().y)))
Monitor* WM::getMonitorForRectangle( point xy, point wh ){
	Monitor *returnMonitor = selectedMonitor_;
	int area = 0;

	for( Monitor& monitor : monitors_ ){
		if( int temp = INTERSECT(xy.x, xy.y, wh.x, wh.y, &monitor) > area ){
			returnMonitor = &monitor;
			area = temp;
		}
	}
	return returnMonitor;
}

bool getRootPtr(point& p){
	int di;
	unsigned int dui;
	Window dummy;
	return XQueryPointer(
			XLib::instance().getDpy(),
			XLib::instance().getRoot(), 
			&dummy, &dummy, 
			&p.x, &p.y, 
			&di, &di, &dui
		);
}

Monitor* WM::getMonitorForWindow( Window w ){
	// check if window is root
	point p;
	if( w == x_.getRoot() && getRootPtr(p) )
		return getMonitorForRectangle(p, {1,1});
	// for each monitor 
	for( auto& monitor : monitors_ ){
		// check if bar window
		if( w == monitor.getBar().getXWindow() )
			return &monitor;
		// check if every client;
		for( auto& ws : monitor.getWorkspaces() )
			for( auto& client : ws.getClients() )
				if( client.getXWindow() == w )
					return &monitor;
	}

	return nullptr; 
}

/* ================================================================================
 *									EVENT
 * =============================================================================== */

void WM::onMotionNotify( const XMotionEvent& ev ){
	LOG("WM recieved: MotionNotify");
	static Monitor *mon = NULL;
	Monitor* m;
	if (ev.window != x_.getRoot())
		return;
	if ( ( m = getMonitorForRectangle( {ev.x_root, ev.y_root},{ 1, 1 }))  != mon && mon) {
		unfocusClient( selectedMonitor_->getSelectedClient() );
		selectedMonitor_ = m;
	}
}

void WM::onProperityNotify( const XPropertyEvent& ev ){
	LOG("WM recieved: PropertyNotify ");
	if( ev.window == x_.getRoot() && ev.atom == XA_WM_NAME ){
		LOG("	root name / status string" );
		selectedMonitor_->updateBar();	
	}
	else if( ev.window == selectedMonitor_->getBar().getXWindow() ){
		selectedMonitor_->getBar().setStatusString( 
				xatoms_.getTextProperity( selectedMonitor_->getBar().getXWindow(), NetWMName) );
		LOG("	bar - " );
	}
	else if( auto* client = getClientForWindow( ev.window ) ){
		LOG("	client - " );
		if( ev.atom ==  XA_WM_TRANSIENT_FOR ){
			LOG( "		TRANSIENT FOR - unimpl");	
		}
		else if( ev.atom ==  XA_WM_NORMAL_HINTS ){
			LOG( "		WM_NORMAL_HINTS - unimpl ");	
		}
		else if( ev.atom ==  XA_WM_HINTS ){
			LOG( "		WM_HINTS -unimpl ");	
		}
		else if( ev.atom == XA_WM_NAME || ev.atom == x_.getAtom(NetWMName) ) {
			LOG( "		WM_NAME ");	
			selectedMonitor_->getBar().setTitle( xatoms_.getTextProperity( ev.window, XA_WM_NAME ) );
			selectedMonitor_->updateBar();
		}
		else if (ev.atom == x_.getAtom(NetWMWindowType)){
			LOG( "		WM_Window_type  -ul");	
		}
	}
}

void WM::onConfigureNotify( const XConfigureEvent& ev ) {
	LOG("WM received: ConfigureNotify ");
	if( ev.window == x_.getRoot() ){
		LOG("	root reconfigure, unimplemented");
	}
} 

void WM::onButtonPress( const XButtonPressedEvent&  ev ){
	ClickLocation cl;
	// focus monitor if neccessary
	// if bar window clicked
	LOG("WM received: ButtonPress Event for window " << ev.window);
	if( ev.window == selectedMonitor_->getBar().getXWindow() ){
		LOG("	 -bar - unimplemented");
		cl = ClickLocation::TitleBar;
	}
	// if some client is clicked
	else if ( auto* client = getClientForWindow( ev.window ) ){
		LOG("	-client - unimplemented");
		cl = ClickLocation::Client;
	}
	// run a command for that click
	auto it = std::find_if( config.mouseBindings.begin(), config.mouseBindings.end(), 
		[&]( const MouseBinding& mb ){
			return 
				mb.click == cl && 
				mb.button == ev.button && 
				xutil::cleanMask( ev.state ) == xutil::cleanMask(mb.mask);
		});

	if( it != config.mouseBindings.end() )
		it->action( it->arg );
} 

void WM::onExpose( const XExposeEvent& e ){
	LOG("WM received: expose event");
	Monitor* m;
	if ( e.count == 0 && (m = getMonitorForWindow(e.window))  ) {
		LOG("	updating bar");
		m->updateBar();
	}
}

void WM::onEnterNotify( const XCrossingEvent& ev  ){ 
	LOG("WM received: EnterNotify");
	if ((ev.mode != NotifyNormal || ev.detail == NotifyInferior) && ev.window != x_.getRoot())
		return;
	
	if( auto* c = getClientForWindow(ev.window) ){
		// get monitor for client
		// TODO: getMonitorForWindow will never be executed
		auto *m = c ? &c->getWorkspace().getMonitor() : getMonitorForWindow(ev.window);
		// if selected client is alredy entered client, no action
		if( &m->getSelectedClient() == c )
			return;
		// else unfocus current selected client
		if( selectedMonitor_->hasSelectedClient() )
			unfocusClient(selectedMonitor_->getSelectedClient());
		// select new monitor if needed
		if( m != selectedMonitor_ ){
			selectedMonitor_ = m;
		}
		// set selected client iterator in workspace
		selectedMonitor_->getSelectedWorkspace().setSelectedClient( *c );
		focusClient(*c);
	}
}


void WM::onClientMessage( const XClientMessageEvent& e ){
	LOG("WM received: ClientMessage Event");
	// 2 messages aceppted
	if( auto* c = getClientForWindow(e.window) ) {
		// NetWMState -> modify fullscreen state of client
		if( e.message_type == x_.getAtom(NetWMState) ) {
			if ( e.data.l[1] == static_cast<long>(x_.getAtom(NetWMFullscreen)) || 
				 e.data.l[2] == static_cast<long>(x_.getAtom(NetWMFullscreen)) ){
				// data.l[0] ==  1/2 netwmstateADD/toggle
				c->setFullscreen( 
						   (e.data.l[0] == 1 || 
						   (e.data.l[0] == 2 && !c->isFullscreen()))
						);
			}
		// NerWMActiveWindow -> received when a client wants attention / is urgent
		} else if (e.message_type == x_.getAtom(NetActiveWindow)) {
			if (c != &selectedMonitor_->getSelectedWorkspace().getSelectedClient() && !c->isUrgent())
				c->setUrgent( true );
		}
	}
} 

void WM::onUnmapNotify( const XUnmapEvent& e ){ 
	LOG("WM received: UnmapNotify" << e.window);
	Client *c;

	if ((c = getClientForWindow(e.window))) {
		LOG("	Found client! unmap came from send event:" << e.send_event );
		// true if this event came from XSendEvent
		if (e.send_event)
			c->setState(WithdrawnState);
		else
			unmanageClient(*c);
	}
} 

void WM::onMappingNotify( XMappingEvent& e ) {
	LOG( "WM received: MappingNotify" );
	XRefreshKeyboardMapping( &e );
	if( e.request == MappingKeyboard )
		grabKeys( );
}

void WM::onConfigureRequest( const XConfigureRequestEvent& e) {
	// check if window has a client and update client
	if( auto* c = getClientForWindow( e.window ) ) {
		LOG("WM received: ConfigureRequest Event | existing client - UNIMPLEMENTED");
	}
	else {
		LOG( "WM received: ConfigureRequest Event | new client" );
		XWindowChanges changes{
			.x=e.x, .y=e.y,
			.width=e.width, .height=e.height,
			.border_width=e.border_width,
			.sibling = e.above,
			.stack_mode = e.detail,
		};
		x_.configureWindow( e.window,  changes, e.value_mask );
	}
	x_.sync( False );
}

void WM::onMapRequest( const XMapRequestEvent& e ) {
	LOG( "WM received: MapRequest Event - " << (
		( getClientForWindow(e.window) ? "already handled " : "new window " ) ) << e.window );
	// dont map windows with existing clients
	if( getClientForWindow(e.window) )
		return;
	auto& ws = selectedMonitor_->getSelectedWorkspace();
	if( ws.hasSelectedClient() ) 
		unfocusClient( ws.getSelectedClient() );
	ws.addClient( e.window );
	ws.focusFront(); // reset focus iterator after client list change, necessary
	LOG("HERE");
	arrangeClients(ws);
	x_.mapWindow( e.window );
	x_.selectInput( e.window , EnterWindowMask|FocusChangeMask|PropertyChangeMask|StructureNotifyMask);
	focusClient ( ws.getClients().front() );
	grabButtons ( ws.getClients().front() );
	selectedMonitor_->updateBar();
}

void WM::onKeyPress( const XKeyEvent& e ){
	auto key = std::find_if( config.keybindings.begin(), config.keybindings.end(), 
		[&]( const KeyBinding& kb ){
			return x_.keySymToKeyCode( kb.keySym ) == e.keycode && 
				   static_cast<int>(e.state) == kb.mod;
		});
	if( key != config.keybindings.end() )
		key->action( key->a );
	LOG("WM received: KeyPress Event" << key->keySym);
}

void WM::onDestroyNotify( const XDestroyWindowEvent& e ){
	// received after unmap
	LOG( "WM received: DestroyNotify: " << e.window );
	if( auto* c = getClientForWindow( e.window ) ) {
		LOG( "\tfound client for DestroyNotify window" );
		auto& ws = c->getWorkspace();
		unmanageClient( *c );
		if( ws.getClients().size() != 0 ){
			ws.focusFront();
			focusClient( ws.getClients().front() ); 
		}
		selectedMonitor_->updateBar();
	}
}


void WM::onFocusIn( const XFocusChangeEvent&  e){
	LOG("WM received: FocusIn");
	if( selectedMonitor_->hasSelectedClient()) {
		auto& client =  selectedMonitor_->getSelectedClient();
		if( e.window != client.getXWindow() ){
			client.takeInputFocus();
			if( client.isFloating() ){
				client.raise();
			}
		}
	}
}

/* ================================================================================
 *									UTIL
 * =============================================================================== */

inline void WM::arrangeClients( Workspace& ws ){ 
	LOG("ARRANGE");
	ws.arrangeClients( 
		(ws.getMonitor().getBar().isHidden() ? 0 : ws.getMonitor().getBar().getHeight()  ) ,
		ws.getMonitor().getSize()
	);
}

void WM::grabKeys() {
	for( const auto& kb : config.keybindings ) {
		x_.grabKey( kb.keySym, kb.mod );
	}
}

void WM::grabButtons( Client& c ) {
	// client specific, cant grab keys for root only
	for( const auto& kb : config.mouseBindings ) {
		x_.grabButton(c.getXWindow(), kb.button, kb.mask );
	}
}

void WM::focusClient( Client& client ) {
	// XSendEvent XSetInputFocus  & NEtACtive
	client.takeInputFocus();	
	client.setBorderColor( Client::config.selecetedBorderClr );

	// atom update
	xatoms_.setActive( client.getXWindow() );

	// assuming client is on current selected monitor
	std::string title =  xatoms_.getTextProperity( client.getXWindow(), XA_WM_NAME ) ;

	LOG("FOCUS " << title );
	selectedMonitor_->getBar().setTitle( title);
	selectedMonitor_->updateBar();
}

void WM::unfocusClient( Client& client ) {

	x_.setInputFocus( x_.getRoot() );
	xatoms_.removeActive();

	client.setBorderColor( Client::config.borderClr );
	// assuming client is on current selected monitor
	selectedMonitor_->getBar().setTitle( std::string{""} );
	selectedMonitor_->updateBar();
}

Client* WM::getClientForWindow( Window w ){
	for( auto& monitor : monitors_ )
		for( auto& workspace : monitor.getWorkspaces() ) 
			for ( auto& client : workspace.getClients() ) 
				if( client.getXWindow() == w )
					return &client;
	return nullptr;
}

void WM::unmanageClient( Client& c ) {
	auto& ws = c.getWorkspace();
	ws.removeClient( c );
	arrangeClients(ws);

}

void WM::resizeMaster( int i ) {
	selectedMonitor_->getSelectedWorkspace().getConfig().factor+=i;
	selectedMonitor_->getSelectedWorkspace();
	arrangeClients( selectedMonitor_->getSelectedWorkspace() );
}

/* ================================================================================
 *									EVENT
 * =============================================================================== */

void WM::quit(){
	running_ = false;
}

void WM::moveFocus(int i){
	auto& ws = selectedMonitor_->getSelectedWorkspace();
	// unfocus pref
	if (ws.hasSelectedClient() ) 
		unfocusClient( ws.getSelectedClient() );

	// move focus
	ws.moveFocus( i );

	// if focus ok
	if (ws.hasSelectedClient() ) 
		focusClient( ws.getSelectedClient() );
}

void WM::killFocused() {
	auto& ws = selectedMonitor_->getSelectedWorkspace();
	if( not ws.hasSelectedClient() )
		return;

	// if client doesng handle closing itself
	// WM will do it for him
	auto& c = ws.getSelectedClient();
	unfocusClient(c);	
	if( !c.sendEvent( x_.getAtom(WMDelete) ) ) {
		x_.killClient( c.getXWindow() );
	}
	arrangeClients( ws );
}

void WM::goToWorkspace( int i ) {
	// check if index is out of bounds or workspace is alreaty selected
	if( i < 0 || i > 9 || i == selectedMonitor_->getSelectedWorkspaceIndex() ) 
		return; 
	// hide all clients and unfocus current client
	selectedMonitor_->getSelectedWorkspace().hideAllClients();	
	if( selectedMonitor_->getSelectedWorkspace().hasSelectedClient() )
		unfocusClient( selectedMonitor_->getSelectedWorkspace().getSelectedClient() );

	// move to other workspace and focus client
	selectedMonitor_->selectWorkspace(i);
	// all changes to clients on workspace are arranging clients
	// so no need to do it here
	selectedMonitor_->getSelectedWorkspace().showAllClients();
	if( selectedMonitor_->getSelectedWorkspace().hasSelectedClient() )
		focusClient( selectedMonitor_->getSelectedWorkspace().getSelectedClient() );

	// update bar with current 
	selectedMonitor_->updateBar();
}

void WM::moveClient(int i) {
	selectedMonitor_->getSelectedWorkspace().moveSelectedClient(i);
	arrangeClients( selectedMonitor_->getSelectedWorkspace() );
	selectedMonitor_->updateBar();
}

void WM::moveClientToWorkspace( uint i ){
	// if no client is selected or moving client to workspace he's already on no need for move
	if( !selectedMonitor_->getSelectedWorkspace().hasSelectedClient()
		|| i == selectedMonitor_->getSelectedWorkspace().getIndex() )
		return;
	auto& client = selectedMonitor_->getSelectedWorkspace().getSelectedClient();
	unfocusClient( client );
	auto& original_ws = client.getWorkspace();
	auto& destination_ws = selectedMonitor_->getWorkspaces()[i];
	// other ws clients are hidden, so join them
	client.hide();
	// focusing front in both workspaces is doen inside method
	original_ws.moveSelectedClientToWorkspace( destination_ws );

	arrangeClients(original_ws);
		
	if( original_ws.hasSelectedClient() )
		focusClient( original_ws.getSelectedClient() );

	arrangeClients(destination_ws);
	selectedMonitor_->updateBar();
}

void WM::floatToggle(){
	selectedMonitor_->getSelectedWorkspace().getSelectedClient().toggleFloating();
	arrangeClients( selectedMonitor_->getSelectedWorkspace() );
}

void WM::fullscreenToggle(){

	selectedMonitor_->getSelectedWorkspace().toggleFullscreenOnSelectedClient();
	arrangeClients( selectedMonitor_->getSelectedWorkspace() );

	if( auto* c = selectedMonitor_->getSelectedWorkspace().getFullscreenClient() )
		xatoms_.setFullscreen( c->getXWindow(), true );
	else
		xatoms_.setFullscreen( c->getXWindow(), false );
}

void WM::moveFloating() {
	static const auto buttonmask = ButtonPressMask|ButtonReleaseMask;
	static const auto mousemask = (buttonmask|PointerMotionMask);

	Client& client = selectedMonitor_->getSelectedClient();
	if( not client.isFloating() || client.isFullscreen() ) 
		return;
	LOG("MOVE FLOAT");
	XEvent ev;
	int new_x, new_y;
	int ocx = client.getPosition().x;
	int ocy = client.getPosition().y;
	Time lasttime = 0;
	point xy;
	getRootPtr( xy );
	
	if (x_.grabPointer( mousemask)  != GrabSuccess )
		return;
	do {
		x_.maskEvent( ev, mousemask|ExposureMask|SubstructureRedirectMask);
		switch( ev.type ){
			case ConfigureRequest: onConfigureRequest( ev.xconfigurerequest ); break;
			case Expose:           onExpose( ev.xexpose );                     break;
			case MapRequest:       onMapRequest( ev.xmaprequest );             break;
			case MotionNotify: {
				if( ev.type != MotionNotify ) break;
				if ((ev.xmotion.time - lasttime) <= (1000 / 60)) break;
				lasttime = ev.xmotion.time;

				new_x = ocx + (ev.xmotion.x - xy.x);
				new_y = ocy + (ev.xmotion.y - xy.y);
				if( new_x < -client.getSize().x ) new_x = 0;
				if( new_y < -client.getSize().y ) new_y = 0;
				if( new_x > client.getWorkspace().getMonitor().getSize().x ) new_x = client.getWorkspace().getMonitor().getSize().x ;
				if( new_y > client.getWorkspace().getMonitor().getSize().y ) new_y = client.getWorkspace().getMonitor().getSize().x ;
				client.move( {new_x, new_y} );
			}
		}
	} while (ev.type != ButtonRelease);

	x_.ungrabPointer();
}

void WM::resizeFloating() {
	static const auto buttonmask = ButtonPressMask | ButtonReleaseMask;
	static const auto mousemask = buttonmask | PointerMotionMask;

	Client& client = selectedMonitor_->getSelectedClient();
	if( not client.isFloating() || client.isFullscreen() ) 
		return;
	LOG("MOVE FLOAT");
	XEvent ev;
	int new_w, new_h;
	int ocx = client.getPosition().x;
	int ocy = client.getPosition().y;
	Time lasttime = 0;
	point xy;
	getRootPtr( xy );

	if (x_.grabPointer( mousemask)  != GrabSuccess )
		return;
	do {
		x_.maskEvent( ev, mousemask|ExposureMask|SubstructureRedirectMask );
		switch( ev.type ){
			case ConfigureRequest: onConfigureRequest( ev.xconfigurerequest ); break;
			case Expose:           onExpose( ev.xexpose );                     break;
			case MapRequest:       onMapRequest( ev.xmaprequest );             break;
			case MotionNotify: {
				if( ev.type != MotionNotify ) break;
				if ((ev.xmotion.time - lasttime) <= (1000 / 60)) break;
				lasttime = ev.xmotion.time;

				new_w = MAX(ev.xmotion.x - ocx - 2 * client.getBorderWidth() + 1, 1);
				new_h = MAX(ev.xmotion.y - ocy - 2 * client.getBorderWidth() + 1, 1);

				if( new_w < -client.getSize().x ) new_w = 0;
				if( new_h < -client.getSize().y ) new_h = 0;
				if( new_w > client.getWorkspace().getMonitor().getSize().x ) new_w = client.getWorkspace().getMonitor().getSize().x ;
				if( new_h > client.getWorkspace().getMonitor().getSize().y ) new_h = client.getWorkspace().getMonitor().getSize().x ;
				client.resize( {new_w, new_h} );
			}
		}
	} while (ev.type != ButtonRelease);
	x_.ungrabPointer();
}

void WM::toggleBar(){
	LOG( "TOGLE BAR" );
	selectedMonitor_->getBar().toggle();
	arrangeClients( selectedMonitor_->getSelectedWorkspace() );
}

void WM:: moveFocusedToTop(){
	LOG( "MOVE TO TOP" );
	selectedMonitor_->getSelectedWorkspace().moveSelectedClientToTop();
	arrangeClients( selectedMonitor_->getSelectedWorkspace() );
}
