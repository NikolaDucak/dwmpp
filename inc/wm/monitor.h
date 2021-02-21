#pragma once

#include "util/focus_list.h"
#include "wm/workspace.h"
#include "wm/bar.h"
#include <iostream>
#include <X11/extensions/Xinerama.h>

namespace wm {

/**
 * Representation of one physical monitor. Class contains information
 * about pixel dimensions, and origin on the root window of the monitor.
 * Class also provides some basic utility functionality for managing the 
 * contents of the monitor, both clients & workspaces.
 */
class monitor {
public: // static members

    struct config {
        std::vector<std::string> workspaces;
    };

    static const config conf;

    // TODO: WARNING: Not properly tested & not all cases implemented!!!
    /**
     * Utility static method for updating @p monitors list with 
     * information about currently connected physical monitors.
     */
    static bool update_monitors(util::focus_list<monitor>& monitors);

public:

    monitor(uint index, uint width, uint height);

    monitor(uint index, XineramaScreenInfo& info);

    /*
     * wm::workspace class relies on a pointer to monitor for 
     * holding information about parrent monitor
     * so copying and moving is not allowed
     */
    monitor(monitor&) = delete;
    monitor(monitor&&) = delete;
    monitor& operator=(monitor&&) = delete;
    monitor& operator=(monitor&) = delete;

    /**
     * @note Currently, method exists only for the puproses of 
     * changing monitor configuration while dwmpp is running
     */
    void update_rect(const util::rect& x);
    
    /**
     * @note Currently, method exists only for the puproses of 
     * changing monitor configuration while dwmpp is running
     */
    void update_rect(const XineramaScreenInfo& info);

    /**
     * Moves clients from the @p other monitor to this one, placing
     * them in respective workpsaces they were found in on @p other
     * monitor.
     * @note Currently, method exists only for the puproses of 
     * changing monitor configuration while dwmpp is running
     */
    void take_clients(monitor& other);

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

    /**
     * Circulates focus iterator inside `m_workspaces` by @p i.
     */
    void focus_workspace(uint i);

    workspace&  get_workspace(uint i);
    const auto& workspaces() const { return m_workspaces; }
    auto&       workspaces() { return m_workspaces; }
    const auto& rect() const { return m_rect; }
    auto&       bar() { return m_bar; }
    void        update_bar() { m_bar.draw(m_workspaces); }

private:
    
    /**
     * Helper method used for workspace list initialization common for
     * all constructor separated as to not reapat constructor code.
     */
    void init();

    uint                        m_index;
    util::rect                  m_rect;   // size of the monitor in pixels
    //TODO: can it be unified? 
    util::rect                  m_size;   // size of the monitor in pixels
    util::rect                  m_window; // coordinates of pixels on window
    class bar                   m_bar;
    util::focus_list<workspace> m_workspaces;
};

}
