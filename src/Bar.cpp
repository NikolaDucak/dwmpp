#include "Bar.h"

static const int padding = 2;


Bar::Bar( int w ) :
	width_( w )
{
	//TODO: cleanup
	auto* dpy = XLib::instance().getDpy();
	auto screen = XLib::instance().getScreen();

	XSetWindowAttributes wa;
	wa.override_redirect = True;
	wa.event_mask = ButtonPressMask|ExposureMask;
	wa.background_pixmap = ParentRelative;
	
	XClassHint ch = { (char*)"dwm", (char*)"dwm" };
	win_ = XCreateWindow(dpy, DefaultRootWindow(dpy), 
			0, 0, width_, config.font.getHeight(), 0, DefaultDepth(dpy, screen),
			CopyFromParent, DefaultVisual(dpy, screen),
			CWOverrideRedirect|CWBackPixmap|CWEventMask, &wa);
	//XDefineCursor(dpy, m->barwin, cursor[CurNormal]->cursor);
	XMapRaised(dpy, win_);
	XSetClassHint(dpy, win_, &ch);
}

void Bar::toggle() { 
	hidden_ = !hidden_;  
	if( hidden_ )
		XLib::instance().moveWindow( win_, 0, -config.font.getHeight() );
	else 
		XLib::instance().moveWindow( win_, 0, 0 );
}

void Bar::draw( std::array<Workspace,10>& workspaces , uint selectedWorkspaceIndex )
{
	static XGraphics graphics( XLib::instance() );
	auto& font = config.font;
	graphics.fillRectangle( 
			config.barBG, {0,0} , {width_, config.font.getHeight()} );

	int ws_tag_start = 0;	
	int ws_tag_inc = config.font.getHeight();

	int cnt = 0;
	for( auto& workspace : workspaces ){
		if( workspace.getIndex() >= Workspace::config.workspaces.size() )
			break;
		const std::string& ws_name = Workspace::config.workspaces[ workspace.getIndex() ];
		int ws_width = config.font.getTextWidthInPixels( ws_name );
		ws_tag_inc = ws_width + padding*2;
		
		// selected Workspace
		if( workspace.getIndex() == selectedWorkspaceIndex )
			graphics.fillRectangle( 
					config.selectedBarBG, {ws_tag_start,0} , {ws_tag_inc, font.getHeight()} );
		// has items
		else if ( not workspace.getClients().empty() )
			graphics.fillRectangle( 
					config.usedTagBG, {ws_tag_start,0} , {ws_tag_inc, font.getHeight()} );
		// no items;
		else
			graphics.fillRectangle( 
					config.emptyTagBG, {ws_tag_start,0} , {ws_tag_inc, font.getHeight()} );

		ws_tag_start += ws_tag_inc;
	}
	ws_tag_start = 0;	
	graphics.copyArea( win_ ,{0,0}, {width_,font.getHeight()} );

	cnt = 0;
	for( auto& workspace : workspaces ){
		const Color* clr = nullptr;
		if( workspace.getIndex() >= Workspace::config.workspaces.size() )
			break;
		const std::string& ws_name = Workspace::config.workspaces[ workspace.getIndex() ];
		if( workspace.getIndex() == selectedWorkspaceIndex )
			clr = &config.selectedBarFG;
		// has items
		else if ( not workspace.getClients().empty() )
			clr = &config.usedTagFG;
		// no items;
		else
			clr = &config.emptyTagFG;

		int ws_width = font.getTextWidthInPixels( ws_name );
		ws_tag_inc = ws_width + padding*2;
		graphics.drawText( win_, config.font, *clr, 
				{ ws_tag_start+padding,0 }, 
				ws_name );
		ws_tag_start += ws_tag_inc;
	}

	graphics.drawText( win_, config.font, config.selectedBarFG, 
			{ws_tag_start+10,0}, 
			titleString_ );
	graphics.drawText( win_, config.font, config.barFG, 
			{width_ - font.getTextWidthInPixels( statusString_ ) ,0 }, 
			statusString_);
}
