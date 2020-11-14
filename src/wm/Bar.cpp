#include "wm/Bar.h"


Bar::Bar() :
    m_hidden(false),
    m_width(1200), //TODO: dont hard code 
    m_xwin(0,0,1280,config.font.getHeight()),
    m_status("TEMPRORAY STATUS"),
    m_title("") 
{ 
   m_xwin.mapRaised(); 
}

void Bar::draw(const std::array<Workspace,10>& workspaces, uint selectedWSIndex) {
	static xlib::XGraphics graphics{};

    int height = config.font.getHeight();

	graphics.fillRectangle( 
			config.barBG , {0,0} , {m_width, height} );

    point tag_position { 0, 0 };
    point tag_size { height, height };
    int next_tag_position_inc = height;

    for (const auto& ws : workspaces) {
        const xlib::XColor *tag_bg;

        if (ws.getIndex() == selectedWSIndex)
            tag_bg = &config.selectedTagBG;
        else if (ws.empty())
            tag_bg = &config.emptyTagBG;
        else 
            tag_bg = &config.usedTagBG;

        graphics.fillRectangle(*tag_bg, tag_position, tag_size);
        tag_position.x += next_tag_position_inc;
    }

    graphics.copyArea(m_xwin.get(), { 0, 0 }, { m_width, height });
    graphics.drawText(m_xwin, config.font, config.barFG,
                      tag_position,  // writing to position of last tag
                      m_title);
    graphics.drawText(m_xwin, config.font, config.barFG,
        { m_width - config.font.getTextWidthInPixels(m_status), 0 }, 
        m_status);
}

