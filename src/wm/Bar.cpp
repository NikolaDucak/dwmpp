#include "wm/Bar.h"


void Bar::configure(const config::BarConfig&) {
    config.barBG = xlib::XColor("#aaaaaa");
    config.barFG = xlib::XColor("#444444");
    config.font = xlib::XFont("Iosevka");
}

Bar::Bar() :
    m_hidden(false),
    m_width(1280), //TODO: dont hard code debug value
    m_xwin(0,0,1280,10),
    m_status(""),
    m_title("TEMPORARY TITLE") 
{ 
   m_xwin.mapRaised(); 
}

/*
void Bar::updateTitle() {
    // get text properity of active window
    m_xwin.xcore->getTextProperity();
}
*/

void Bar::updateStatus() {
    //get text properity of root
}

void Bar::redraw() {
	static xlib::XGraphics graphics{};

	graphics.fillRectangle( 
			config.barBG , {0,0} , {m_width, 10} );

	int ws_tag_inc = 10;

	int cnt = 0;
    point ws_tag_position{ 0, ws_tag_inc };

	graphics.copyArea( m_xwin.get() ,{0,0}, {m_width,10} );

    ws_tag_position.x += 0;
	cnt = 0;

	graphics.drawText( m_xwin, config.font, config.barFG, 
            {0,0},  // writing to position of last tag
			m_title);
    /*
	graphics.drawText( m_xwin, config.font, config.barFG, 
			{m_width - font.getTextWidthInPixels( m_status) ,0 }, 
			m_status);
    */
}
