#include "wm/bar.h"
#include "util/focus_list.h"
#include "xlib/XColor.h"
#include "xlib/XFont.h"

namespace wm {

std::pair<std::vector<int>, std::vector<int>>
precompute_tag_info(util::focus_list<workspace>& workspaces,
                    const xlib::XFont&                 font) {
    std::vector<int> tag_widths;
    std::vector<int> tag_text_x_offsets;

    for (const auto& ws : workspaces) {
        auto& ws_name = workspace::conf.workspaces[ws.get_index()];
        // match size of tag bg rect with 
        auto name_w = font.getTextWidthInPixels(ws_name);

        // center text when forcing square tag rect
        if (name_w < font.getHeight()) {
            tag_widths.push_back(font.getHeight());
            tag_text_x_offsets.push_back(font.getHeight()/2 - name_w/2);
        } else {
            tag_widths.push_back(name_w);
            tag_text_x_offsets.push_back(0);
        }
    }
    return { tag_widths, tag_text_x_offsets };
}

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
    // hiding is implemented as moving window position
    // to beyond screen borders
    m_xwindow.moveWindow(0, -1 - conf.font.getHeight());
    m_visible = false;
}

void bar::show() {
    // return window to original position
    m_xwindow.moveWindow(0, 0);
    m_visible = true;
}

void bar::draw(util::focus_list<workspace>& workspaces) {
    static xlib::XGraphics graphics {};
    using util::point;

    // if bar is hidden don't bother drawing it
    if (not m_visible) return;

    // height of bar & dimestions of tag square
    static const int height = conf.font.getHeight();

    // dimesions of workspace tag square
    point tag_size { height, height };

    // first workspace tag square x,y location
    point tag_position { 0, 0 };

    // fill bar background
    graphics.fillRectangle(conf.barBG, { 0, 0 }, { (int)m_width, height });

    // space between tag markers
    static const int tag_padding = 2;

#if 0
    // one bar belong to one monitor and ownership wont change, besides
    // that, workspace number or names wont change at runtime

    // draw all workspace tag quares
    for (const auto& ws : workspaces) {
        const xlib::XColor *tag_bg, *tag_fg;
        auto& ws_name = workspace::conf.workspaces[ws.get_index()];

        // select tag square color
        if (ws.get_index() == workspaces.focused()->get_index()) {
            tag_bg = &conf.focused_tag_bg;
            tag_fg = &conf.focused_tag_fg;
        } else if (ws.empty()) {
            tag_bg = &conf.empty_tag_bg;
            tag_fg = &conf.empty_tag_fg;
        } else {
            tag_bg = &conf.unfocused_tag_bg;
            tag_fg = &conf.unfocused_tag_fg;
        }

        // match size of tag bg rect with 
        tag_size.x = conf.font.getTextWidthInPixels(ws_name);

        // center text when forcing square tag rect
        if (tag_size.x < tag_size.y) {
            auto text_w = tag_size.x;
            tag_size.x = tag_size.y;
            auto text_pos = tag_position;
            text_pos.x += tag_size.x/2 - text_w/2;
            graphics.fillRectangle(*tag_bg, tag_position, tag_size);
            graphics.drawText(conf.font, *tag_fg, text_pos, ws_name);
        } else {
            graphics.fillRectangle(*tag_bg, tag_position, tag_size);
            graphics.drawText(conf.font, *tag_fg, tag_position, ws_name);
        }

        // move tag position to next tag that's gonna be drawn
        tag_position.x += tag_size.x + tag_padding;
    }
#else
    static const auto pair =
        precompute_tag_info(workspaces, conf.font);

    static const auto tag_widths = pair.first;
    static const auto tag_text_x_offsets = pair.second;

    for (const auto& ws : workspaces) {
        const xlib::XColor *tag_bg, *tag_fg;
        auto& ws_name = workspace::conf.workspaces[ws.get_index()];
        auto tag_w = tag_widths[ws.get_index()];
        tag_size.x = tag_w;

        // select tag square color
        if (ws.get_index() == workspaces.focused()->get_index()) {
            tag_bg = &conf.focused_tag_bg;
            tag_fg = &conf.focused_tag_fg;
        } else if (ws.empty()) {
            tag_bg = &conf.empty_tag_bg;
            tag_fg = &conf.empty_tag_fg;
        } else {
            tag_bg = &conf.unfocused_tag_bg;
            tag_fg = &conf.unfocused_tag_fg;
        }

        graphics.fillRectangle(*tag_bg, tag_position, tag_size);
        auto text_pos = tag_position;
        text_pos.x += tag_text_x_offsets[ws.get_index()];
        graphics.drawText(conf.font, *tag_fg, text_pos, ws_name);

        tag_position.x += tag_w + tag_padding;
    }

#endif


    //TODO: length check for drawing realy titles
    // display title of active window in the middle of the bar
    int title_left = (m_width - conf.font.getTextWidthInPixels(m_title))/2;
    graphics.drawText(conf.font, conf.barFG, point { title_left, 0 }, m_title);

    // display status in the rightmost corner of bar
    int status_left = m_width - conf.font.getTextWidthInPixels(m_status);
    graphics.drawText(conf.font, conf.barFG, point { status_left, 0 }, m_status);

    // display drawn bar on bar window
    graphics.copyArea(m_xwindow.get(), { 0, 0 }, { (int)m_width, height });
}

void bar::toggle_visibility() {
    if (m_visible)
        hide();
    else
        show();
}

}  // namespace wm
