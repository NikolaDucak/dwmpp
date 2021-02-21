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
        xlib::XFont font;
        xlib::XColor barBG, barFG; 
        xlib::XColor empty_tag_bg, empty_tag_fg;
        xlib::XColor focused_tag_bg, focused_tag_fg;
        xlib::XColor unfocused_tag_bg, unfocused_tag_fg;
    };
    static const config conf;

public:
    bar(uint width);

    void set_title_string(const std::string& str) { m_title = str; }
    void set_status_string(const std::string& str) { m_status = str; }
    void toggle_visibility();
    void hide();
    void show();
    bool is_visible() const { return m_visible; }
    void draw(const util::focus_list<workspace>& workspaces) const;
    void update_width(uint w);
    auto get_raw_xwindow() { return m_xwindow.get(); }

private:
    bool m_visible;
    uint m_width;
    xlib::XWindow m_xwindow;
    std::string m_title, m_status;
};


}  // namespace wm
