#include "wm/Bar.h"


Bar::Bar() :
    m_hidden(false),
    m_width(1200), //TODO: dont hard code 
    m_xwin(0,0,1280,config.font.getHeight()),
    m_status("dwmpp 0.01"),
    m_title("") 
{ 
    // map window to root surface and raise it above other windows
    m_xwin.mapRaised();
    // tell x that for bar window only mouse button event and expose events
    // shall be generated
    m_xwin.selectInput(ButtonPressMask | ExposureMask);
}

void Bar::toggleHidden() {
    m_hidden = !m_hidden;

    if (m_hidden) {
        m_xwin.moveWindow(0, -40); 
    } else {
        m_xwin.moveWindow(0, 0);
    }
}

void Bar::draw(const std::array<Workspace,10>& workspaces, uint selectedWSIndex) {
	static xlib::XGraphics graphics{};
    
    // if bar is hidden don't bother drawing it
    if (m_hidden) return;

    // height of bar & dimestions of tag square
    static const int height = config.font.getHeight();

    // dimesions of workspace tag square
    static const point tag_size { height, height };

    // first workspace tag square x,y location
    point tag_position { 0, 0 };

    // distance from left corner of one tag to left corner of other
    int next_tag_position_inc = height + 1;

    // fill bar background
	graphics.fillRectangle( 
			config.barBG , {0,0} , {m_width, height} );

    // draw all workspace tag quares
    for (const auto& ws : workspaces) {
        const xlib::XColor *tag_bg;

        // select tag square color
        if (ws.getIndex() == selectedWSIndex)
            tag_bg = &config.selectedTagBG;
        else if (ws.empty())
            tag_bg = &config.emptyTagBG;
        else 
            tag_bg = &config.usedTagBG;

        // draw tag square
        graphics.fillRectangle(*tag_bg, tag_position, tag_size);

        // move tag position to next tag that's gonna be drawn
        tag_position.x += next_tag_position_inc;
    }

    // display drawn bar on bar window
    graphics.copyArea(m_xwin.get(), { 0, 0 }, { m_width, height });

    // display title of active window after workspace tags
    auto title_left = (m_width - config.font.getTextWidthInPixels(m_title))/2;
    graphics.drawText(m_xwin, config.font, config.barFG,
                      //tag_position,  // writing to position of last tag
                      point{title_left,0}, // writing in the middle
                      m_title);

    // display status in the rightmost corner of bar
    graphics.drawText(m_xwin, config.font, config.barFG,
        { m_width - config.font.getTextWidthInPixels(m_status), 0 }, 
        m_status);
}

