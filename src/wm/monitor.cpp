#include "wm/monitor.h"

#include "xlib/XCore.h"
#include <X11/Xlib.h>
#include  <X11/extensions/Xinerama.h>

namespace wm {

namespace { // anonymous 

static bool is_there_difference_in_monitor_rect(XineramaScreenInfo& x, monitor& m) {
    return x.x_org != m.rect().top_left.x || x.y_org != m.rect().top_left.y ||
           x.width != (short)m.rect().width || x.height != (short)m.rect().height;
}

static int is_unique_geom(XineramaScreenInfo unique[], size_t n,
                        XineramaScreenInfo& info) {
    while (n--)
        if (unique[n].x_org == info.x_org && unique[n].y_org == info.y_org &&
            unique[n].width == info.width && unique[n].height == info.height)
            return 0;
    return 1;
}

} // anonymous namespace

bool monitor::update_monitors(util::focus_list<monitor>& monitors) {
    auto screen = xlib::XCore::instance().getScreen();
    auto dpy = xlib::XCore::instance().getDpyPtr();
    auto screen_w = DisplayWidth(dpy, screen);
    auto screen_h = DisplayHeight(dpy, screen);
    bool dirty = false;

    if (XineramaIsActive(dpy)) {
        int                 new_monitors;
        int                 old_monitors = monitors.size();
        XineramaScreenInfo* info{ XineramaQueryScreens(dpy, &new_monitors) };

        /* only consider unique geometries as separate screens */
        auto unique = new XineramaScreenInfo[new_monitors];
        int i, j;
        for (i = 0, j = 0; i < new_monitors; i++)
            if (is_unique_geom(unique, j, info[i]))
                unique[j++] = info[i];
        XFree(info);
        new_monitors = j;

        if (old_monitors <= new_monitors) { /* new monitors available */
            dirty  = true;
            // append new monitors to the monitor list
            for (i = 0; i < (new_monitors - old_monitors); i++)
                monitors.emplace_back((uint)new_monitors + i, unique[i]);

            auto m = monitors.begin();
            // checking if old monitors have changed their dimensions
            for (i = 0; i < old_monitors && m != monitors.end(); i++, m++) {
                if (is_there_difference_in_monitor_rect(unique[i], *m))
                    m->update_rect(unique[i]);
            }
        } else { /* less monitors available */
            //TODO:
            // remove monitoirs from monitor list
            auto old_monitor = monitors.end();
            old_monitor--;
            for(int i = old_monitors; i > new_monitors; i--){
                monitors.front().take_clients(*old_monitor);
                old_monitor--;
            }
            // merge old monitors workspaces to existing monitor
        }
    } else {  
        /* default monitor setup */
        if (monitors.empty()) { monitors.emplace_back(0, screen_w, screen_h); }
        //TODO: else check size
        dirty = true;
    }
    if (dirty) {
        monitors.focus_front();
    }

    return dirty;
}

monitor::monitor(uint index, XineramaScreenInfo& info) :
    m_index { index },
    m_rect { { info.x_org, info.y_org },
             static_cast<unsigned>(info.width),
             static_cast<unsigned>(info.height) },
    m_bar { (uint)info.width } {
    init();
}

monitor::monitor(uint index, uint width, uint height) :
    m_index { index },
    m_rect { util::point { 0, 0 }, width, height },
    m_bar { width },
    m_workspaces {} {
    init();
}

void monitor::init() {
    // populate `m_workspaces` with number of workspaces from config
    for (size_t i = 0; i < conf.workspaces.size(); i++) {
        m_workspaces.emplace_back(this, i);
    }

    // focus firtst workspace
    m_workspaces.focus_front();

    // draw bar
    m_bar.draw(m_workspaces);
}

void monitor::focus_workspace(uint i) {
    m_workspaces.focused()->hide_clients();
    m_workspaces.focus_front();
    // 1+i since actual indexes start with 0
    m_workspaces.circulate_focus(i-1); 
    m_workspaces.focused()->show_clients();
    m_bar.draw(m_workspaces);
}

void monitor::focus() {
    m_workspaces.focused()->focus();
    m_bar.show();
}

void monitor::unfocus() {
    m_workspaces.focused()->unfocus();
    m_bar.hide();
}

workspace& monitor::get_workspace(uint i) {
    auto temp = m_workspaces.begin();
    // workspaces, from user config side are numbered from 1
    // so correction by -1 is needed to get correct index of workspace
    // wich means by decrementing before checking value we get desired result
    while (--i) temp++;
    return *temp; 
}

void monitor::update_rect(const util::rect& x) {
    m_rect = x;
    m_bar.update_width(x.width);
}

void monitor::update_rect(const XineramaScreenInfo& info) {
    m_rect = { { info.x_org, info.y_org },
               (unsigned)info.width,
               (unsigned)info.height };
    m_bar.update_width(info.width);
    
}

void monitor::take_clients(monitor& other) {
    auto ws = m_workspaces.begin();
    auto ows = other.m_workspaces.begin();
    // different number of workspaces should not be possible
    // therfore checking just one iterator for end is fine
    while (ws!=m_workspaces.end()) {
        ws->take_clients(*ows);
        ws++;
        ows++;
    }
}

}  // namespace wm
