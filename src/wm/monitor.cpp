#include "wm/monitor.h"

namespace wm {

void monitor::update_monitors(util::focus_list<monitor>& monitors) {
    monitors.emplace_back(0, 1200,700);
}

monitor::monitor(uint index, uint width, uint height) :
    m_index { index },
    m_rect { util::point { 0, 0 }, width, height },
    m_bar { width },
    m_workspaces {} {
    // set number of workspaces from config
    for (size_t i = 1; i <= workspace::conf.workspaces.size(); i++) {
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
    i--;
    auto temp = m_workspaces.begin();
    while(i--) temp++;
    return *temp; 
}

}  // namespace wm
