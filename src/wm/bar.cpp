#include "wm/bar.h"


namespace wm {

bar::bar(uint width) :
    m_visible(true),
    m_width(width),
    m_xwindow(0, 0, width, conf.font.getHeight()),
    m_title(""),
    m_status("dwmpp 0.01") {
    // map window to root surface and raise it above other windows
    m_xwindow.mapRaised();
    // tell x that for bar window only mouse button event and expose events
    // shall be generated
    m_xwindow.selectInput(ButtonPressMask | ExposureMask);
}

void bar::hide() {
    m_xwindow.moveWindow(0, -1 - conf.font.getHeight());
    m_visible = true;
}

void bar::show() {
    m_xwindow.moveWindow(0, 0);
    m_visible = false;
}

void bar::draw(util::focus_list<workspace>& workspaces) {
    static xlib::XGraphics graphics {};
    using util::point;

    // if bar is hidden don't bother drawing it
    if (not m_visible) return;

    // height of bar & dimestions of tag square
    static const int height = conf.font.getHeight();

    // dimesions of workspace tag square
    static const point tag_size { height, height };

    // first workspace tag square x,y location
    point tag_position { 0, 0 };

    // distance from left corner of one tag to left corner of other
    int next_tag_position_inc = height + 1;

    // fill bar background
    graphics.fillRectangle(conf.barBG, { 0, 0 }, { (int)m_width, height });

    // draw all workspace tag quares
    for (const auto& ws : workspaces) {
        const xlib::XColor *tag_bg;

        // select tag square color
        if (ws.get_index() == workspaces.focused()->get_index())
            tag_bg = &conf.focused_tag_bg;
        else if (ws.empty())
            tag_bg = &conf.empty_tag_bg;
        else 
            tag_bg = &conf.unfocused_tag_bg;

        // draw tag square
        graphics.fillRectangle(*tag_bg, tag_position, tag_size);

        // move tag position to next tag that's gonna be drawn
        tag_position.x += next_tag_position_inc;
    }

    // display drawn bar on bar window
    graphics.copyArea(m_xwindow.get(), { 0, 0 }, { (int)m_width, height });

    // display title of active window in the middle of the bar
    int title_left = (m_width - conf.font.getTextWidthInPixels(m_title))/2;
    graphics.drawText(m_xwindow, conf.font, conf.barFG, point { title_left, 0 }, m_title);

    // display status in the rightmost corner of bar
    int status_left = m_width - conf.font.getTextWidthInPixels(m_status);
    graphics.drawText(m_xwindow, conf.font, conf.barFG, point { status_left, 0 }, m_status);
}

void bar::toggle_visibility() {
    if (m_visible)
        hide();
    else
        show();
}

}  // namespace wm
