#pragma once

#include "util/focus_list.h"
#include "wm/workspace.h"
#include "wm/bar.h"
#include <iostream>

namespace wm {

class monitor {
public: // static 
    static void update_monitors(util::focus_list<monitor>& monitors);

public:

    monitor(uint index, uint width, uint height);


    workspace&  get_workspace(uint i);
    const auto& workspaces() const { return m_workspaces; }
    const auto& get_area() { return m_area; }
    auto&       workspaces() { return m_workspaces; }
    auto&       bar() { return m_bar; }
    void        update_bar() { m_bar.draw(m_workspaces); }

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
private:
    uint                        m_index;
    util::area                  m_area;
    class bar                   m_bar;
    util::focus_list<workspace> m_workspaces;
};

}
