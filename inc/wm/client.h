#pragma once

#include "util/area.h"
#include "util/point.h"
#include "xlib/XColor.h"
#include "xlib/XWindow.h"

#include <X11/X.h>
#include <unordered_map>

namespace wm {

class workspace;

class client {
public:  // static variables
    struct config {
        unsigned     border_width;
        xlib::XColor focused_border;
        xlib::XColor unfocused_border;
    };

    static const config                        conf;
    static std::unordered_map<Window, client*> window_client_map;

public:
    client(Window w, workspace* parent_workspace);
    ~client();
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

    bool is_floating() const { return m_floating; }
    void toggle_floating();

    /** */
    void set_state(long state);

    void  set_fullscreen(bool f);
    void  set_parent_workspace(workspace* ws);
    auto& get_xwindow() { return m_xwindow; }
    auto& get_parent_workspace() { return *m_parent_workspace; }

    std::string title() const;

    void move(const util::point& where);
    void resize(const util::point& size);
    void move_resize(const util::area& area);

private:
    bool m_floating;
    xlib::XWindow m_xwindow;
    util::point   m_position;
    util::point   m_size;
    workspace* m_parent_workspace;
};

}  // namespace wm
