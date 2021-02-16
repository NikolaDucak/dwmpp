#pragma once

#include "util/focus_list.h"
#include "wm/workspace.h"
#include "wm/bar.h"
#include <iostream>
#include <X11/extensions/Xinerama.h>

namespace wm {

class monitor {
public: // static 
    static bool update_monitors(util::focus_list<monitor>& monitors);

public:

    monitor(uint index, uint width, uint height);

    monitor(uint index, XineramaScreenInfo& info);

    // wm::workspace class relies on a pointer to monitor for 
    // holding information about parrent monitor
    // so copying and moving is not allowed
    monitor(monitor&) = delete;
    monitor(monitor&&) = delete;
    monitor& operator=(monitor&&) = delete;
    monitor& operator=(monitor&) = delete;

    void update_rect(const util::rect& x) {
        m_rect = x;
        m_bar.update_width(x.width);
    }

    void update_rect(const XineramaScreenInfo& info) {
        m_rect = { { info.x_org, info.y_org },
                   (unsigned)info.width,
                   (unsigned)info.height },
        m_bar.update_width(info.width);
    }

    void take_clients(monitor& other) {
        // different number of workspaces should not be possible
        auto ws = m_workspaces.begin();
        auto ows = other.m_workspaces.begin();
        while(ws!=m_workspaces.end()) {
            ws->take_clients(*ows);
            ws++;
            ows++;
        }
    }
    /**
     * Calls workspace::unfocus() method on current workspace
     * to drop input focus from current client then
     * removes title & status strings from the bar
     */
    void unfocus();

    /**
     * Calls workspace::focus() method on current workspace
     * to give it input focus and sets title & status string
     * to the bar
     */
    void focus();

    void focus_workspace(uint i);

    workspace&  get_workspace(uint i);
    const auto& workspaces() const { return m_workspaces; }
    const auto& rect() const { return m_rect; }

    auto&       workspaces() { return m_workspaces; }
    auto&       bar() { return m_bar; }
    void        update_bar() { m_bar.draw(m_workspaces); }

private:
    uint                        m_index;

    util::rect                  m_rect;   // size of the monitor in pixels
    //TODO: can it be unified
    util::rect                  m_size;   // size of the monitor in pixels
    util::rect                  m_window; // coordinates of pixels on window
    class bar                   m_bar;
    util::focus_list<workspace> m_workspaces;
};

}
