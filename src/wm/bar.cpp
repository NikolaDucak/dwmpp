#include "wm/bar.h"
#include "util/focus_list.h"
#include "xlib/XColor.h"
#include "xlib/XFont.h"
#include "wm/monitor.h"
#include <algorithm>

static xlib::XGraphics graphics {};
static std::vector<xlib::XFont> fonts;

namespace wm {

std::pair<std::vector<int>, std::vector<int>>
bar::precompute_tag_info(const util::focus_list<workspace>& workspaces,
                    std::vector<xlib::XFont> &fonts) const {
    std::vector<int> tag_widths;
    std::vector<int> tag_text_x_offsets;

    for (const auto& ws : workspaces) {
        auto& ws_name = monitor::conf.workspaces[ws.get_index()];
        // match size of tag bg rect with text for tag text
        auto name_w = graphics.getTextWidth(ws_name,fonts);

        // center text when forcing square tag rect
        // height is equivalent to the tallest font
        if (name_w < m_height) {
            tag_widths.push_back(m_height);
            tag_text_x_offsets.push_back(m_height/2 - name_w/2);
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
    m_height(std::max_element(conf.fonts.begin(), conf.fonts.end(), [](auto& a, auto& b) { return a.getHeight() < b.getHeight(); })->getHeight()),
    m_xwindow(conf.padding, conf.padding, m_width-conf.padding*2, m_height),
    m_title(""),
    m_status("dwmpp 0.01") {
    // map window to root surface and raise it above other windows
    m_xwindow.mapRaised();
    // tell x that for bar window only mouse button event and expose events
    // shall be generated
    m_xwindow.selectInput(ButtonPressMask | ExposureMask);
    //TODO: remove
    fonts = conf.fonts;
}

void bar::hide() {
    // hiding is implemented as moving window position
    // to beyond screen borders
    m_xwindow.moveWindow(0, -1 - m_height);
    m_visible = false;
}

void bar::show() {
    // return window to original position
    m_xwindow.moveWindow(conf.padding, conf.padding);
    m_visible = true;
}

void bar::draw(const util::focus_list<workspace>& workspaces) const {
    //static xlib::XGraphics graphics {};
    using util::point;

    // if bar is hidden don't bother drawing it
    if (not m_visible) return;

    // height of bar & dimestions of tag square
    static const int height = m_height;

    // dimesions of workspace tag square
    point tag_size { height, height };

    // first workspace tag square x,y location
    point tag_position { 0, 0 };

    // fill bar background
    graphics.fillRectangle(conf.barBG, { 0, 0 }, { static_cast<int>(m_width), height });

    // space between tag markers
    static const int tag_padding { 2 };

    // precomputed dimensions for tags text centering offsets
    static const auto pair =
        precompute_tag_info(workspaces, fonts);
    static const auto tag_widths = pair.first;
    static const auto tag_text_x_offsets = pair.second;

    for (const auto& ws : workspaces) {
        const xlib::XColor *tag_bg, *tag_fg;
        auto& ws_name = monitor::conf.workspaces[ws.get_index()];
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
        // TODO: remove temporary fixed 40
        graphics.drawText(text_pos, point { 40, height }, ws_name, fonts, conf.barFG);

        tag_position.x += tag_w + tag_padding;
    }

    // display title of active window in the middle of the bar
    int title_left = (m_width - graphics.getTextWidth(m_title,fonts))/2;

    // display status in the rightmost corner of bar
    int status_left = m_width - graphics.getTextWidth(m_status,fonts) - conf.padding*2;

    graphics.drawText(point { title_left, 0 }, 
                      point { int(status_left - title_left), height }, 
                      m_title, fonts, conf.barFG);
    
    graphics.drawText(point { status_left, 0 }, point { 110, height }, 
                      m_status, fonts, conf.barFG);

    // after bar drawing has been completed, buffer can be dumped to the screen
    graphics.copyArea(m_xwindow.get(), { 0, 0 }, { (int)m_width, height });
}

void bar::toggle_visibility() {
    if (m_visible)
        hide();
    else
        show();
}

void bar::update_width(uint w) {
    m_width = w; 
    m_xwindow.resizeWindow(w, m_height);
}

}  // namespace wm
