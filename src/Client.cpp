#include "Client.h"

Client::Client( Workspace& ws, const Window& w, XWindowAttributes& wa) :
	workspaceRef_(&ws),
	xwin_(w),
	hidden_(false), floating_(false), urgent_(false), fullscreen_(false),
	xy_( {wa.x, wa.y} ), 
	wh_( {wa.width, wa.height} ),
	old_xy_( {wa.x, wa.y} ),
   	old_wh_( {wa.width, wa.height} ),
	bw_(config.borderWidth), old_bw_( wa.border_width)
{ }

void Client::resize( point xy, point wh ) {
	old_xy_ = xy_; 
	old_wh_ = wh_;
	xy_ = xy; 
	wh_ = wh;

	// set window changes
	XWindowChanges wc {
		.x = xy.x,
		.y = xy.y,
		.width = wh.x,
		.height = wh.y,
		.border_width = bw_,
	};

	XlibWrapper::instance().configureWindow( xwin_, wc,
			CWX|CWY|CWWidth|CWHeight|CWBorderWidth);

		
	// config
	XConfigureEvent ce {
		.type = ConfigureNotify,
		.display = XlibWrapper::instance().getDpy(),
		.event = xwin_,
		.window = xwin_,
		.x = xy.x,
		.y = xy.y,
		.width = wh.x,
		.height = wh.y,
		.border_width = bw_,
		.above = None,
		.override_redirect = False
	};

	//TODO: clean up
	XlibWrapper::instance().sendEvent( xwin_, 
			*(XEvent*)&ce,
			StructureNotifyMask);
	XlibWrapper::instance().sync( false );
}

void Client::takeInputFocus() {
	XlibWrapper::instance().setInputFocus( xwin_ );
	XlibWrapper::instance().changeProperty( xwin_, NetActiveWindow, XAWindow,
		reinterpret_cast<unsigned char *>(&xwin_), 1);
	this->sendEvent( XlibWrapper::instance().getAtom( WMTakeFocus) );
}

void Client::hide(){
	hidden_ = true;
	XlibWrapper::instance().moveWindow( xwin_, -(wh_.x+2*20)*2, xy_.y );
}

void Client::show(){
	hidden_ = false;
	XlibWrapper::instance().moveWindow( xwin_, xy_.x , xy_.y );	
}

void Client::setBorderColor( const  Color& c ){
	XlibWrapper::instance().setWindowBorder( xwin_, c.xColor().pixel );
}

bool Client::sendEvent( Atom proto ) {
	int n;
	Atom *protocols;
	bool exists = false;

	// make sure Atom proto is supported by window
	if( XlibWrapper::instance().getWMProtocosl( xwin_, &protocols, &n ) ){
		exists = std::find( protocols, protocols+n, proto ) != protocols+n;
		XFree(protocols);
	}
	// send if supported
	if( exists ) {
		XEvent ev;
		ev.type = ClientMessage;
		ev.xclient.window = xwin_;
		ev.xclient.message_type = XlibWrapper::instance().getAtom(WMProtocols);
		ev.xclient.format = 32;
		ev.xclient.data.l[0] = proto;
		ev.xclient.data.l[1] = CurrentTime;

		XlibWrapper::instance().sendEvent( xwin_,  ev );
	}
	return exists;
}

void Client::setFullscreen( bool f ) { 
	fullscreen_ = f; 
	if( f == false ){
		xy_ = old_xy_;
		wh_ = old_wh_;
		resize( xy_, wh_ );
	}
}

void Client::setState( int state ){
	long data[] = { state, None };
	XlibWrapper::instance().changeProperty( xwin_,
		WMState, WMState, (unsigned char *)data, 2);
}

void Client::move( point xy ){
	resize( xy, wh_ );
}

void Client::resize( point wh ){
	resize( xy_, wh );
}

void Client::raise(){
	XlibWrapper::instance().raiseWindow( xwin_ );
}
