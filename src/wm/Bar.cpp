#include "wm/Bar.h"


Bar::Bar() :
    m_hidden(false),
    m_width(1280), //TODO: dont hard code debug value
    m_xwin(0,0,1280,config.font.getHeight()),
    m_status(""),
    m_title("TEMPORARY TITLE") 
{ 
   m_xwin.mapRaised(); 
}


void Bar::redraw() {
	static xlib::XGraphics graphics{};

    int height = config.font.getHeight();

	graphics.fillRectangle( 
			config.barBG , {0,0} , {m_width, height} );

	int ws_tag_inc = height;

	int cnt = 0;
    point ws_tag_position{ 0, ws_tag_inc };

	graphics.copyArea( m_xwin.get() ,{0,0}, {m_width,height} );

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
