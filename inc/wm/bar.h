#pragma once 

#include "xlib/XFont.h"
#include "xlib/XColor.h"
#include "xlib/XWindow.h"
#include "xlib/XGraphics.h"
#include "workspace.h"
#include "util/focus_list.h"

namespace wm {

class bar {
public: // static
    struct config {
        std::vector<xlib::XFont> fonts;
        xlib::XColor barBG, barFG; 
        xlib::XColor empty_tag_bg, empty_tag_fg;
        xlib::XColor focused_tag_bg, focused_tag_fg;
        xlib::XColor unfocused_tag_bg, unfocused_tag_fg;
    };
    static const config conf;

public:
    bar(uint width);

    void toggle_visibility();
    void hide();
    void show();
    bool is_visible() const { return m_visible; }
    void draw(const util::focus_list<workspace>& workspaces) const;
    void update_width(uint w);

    void set_title_string(const std::string& str) { m_title = str; }
    void set_status_string(const std::string& str) { m_status = str; }

    auto get_raw_xwindow() const { return m_xwindow.get(); }
    auto get_height() const { return m_height; };

private:

    std::pair<std::vector<int>, std::vector<int>>
    precompute_tag_info(const util::focus_list<workspace>& workspaces,
                        std::vector<xlib::XFont>&    fonts) const;

    bool m_visible;
    uint m_width;
    uint m_height;
    xlib::XWindow m_xwindow;
    std::string m_title, m_status;
};


}  // namespace wm
