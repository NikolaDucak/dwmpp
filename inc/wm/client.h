#pragma once

#include "util/area.h"
#include "util/point.h"
#include "xlib/XColor.h"
#include "xlib/XWindow.h"

#include <X11/X.h>
#include <unordered_map>
#include <optional>

namespace wm {

class workspace;

class client {
public:  // static variables
    // TODO: add support for `rules`
    struct config {
        unsigned     border_width;
        xlib::XColor focused_border;
        xlib::XColor unfocused_border;
    };

    //TODO: mabye group `*_w` & `*_h` in util::point?
    struct size_hints {
        int base_w, base_h;
        int min_w, min_h;
        int max_w, max_h;
        int min_a, max_a;
        int inc_w, inc_h;
        bool is_fixed;
    };

    static const config                        conf;
    static std::unordered_map<Window, client*> window_client_map;

public:
    client(Window w, workspace* parent_workspace);
    ~client();

    // window_client_map relies on pointer, so 
    // no memory location changing allowed
    client(client&)  = delete;
    client(client&&) = delete;
    client& operator=(client&) = delete;
    client& operator=(client&&) = delete;

    void take_input_focus();
    void drop_input_focus();

    void kill(); 
    void hide();
    void show();
    void raise();

    void move(const util::point& where);
    void resize(const util::point& size);
    void move_resize(const util::rect& area);

    void update_hints();
    void update_wm_hints(bool is_focused);
    void update_window_type();

    void set_floating(bool f) { m_floating = f; }
    bool is_floating() const { return m_floating; }
    void toggle_floating();

    bool is_fullscreen() const { return m_fullscreen; }
    void set_fullscreen(bool f) { 
        m_fullscreen = f; 
        m_xwindow.setFullscreen(f);
        m_xwindow.setWindowBorderWidth(0);
    }

    bool is_urgent() const { return m_urgent; }
    void set_urgent(bool b) { m_urgent = b; }

    void set_state(long state);

    void  set_parent_workspace(workspace* ws);

    util::point get_size() const { return m_size; }

    std::optional<Window> get_transient_for();

    std::string title() const;

    auto& get_xwindow() { return m_xwindow; }
    auto& get_parent_workspace() { return *m_parent_workspace; }

private:
    bool m_urgent;
    bool m_fullscreen;
    bool m_floating;
    bool m_never_focus;
    xlib::XWindow m_xwindow;
    util::point   m_position;
    util::point   m_size;
    size_hints    m_size_hints;
    workspace*    m_parent_workspace;
};

}  // namespace wm
