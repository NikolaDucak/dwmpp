#include "wm/workspace.h"
#include "wm/monitor.h"
#include "wm/bar.h"

namespace wm {

workspace::workspace(monitor* parent_monitor, unsigned index) :
    m_index(index),
    m_layout(tiling_layout),
    m_parent_monitor(parent_monitor),
    m_clients() {}

void workspace::set_layout(layout_function l) {
    m_layout = l;
    arrange();
}

void workspace::focus_front() {
    if (has_focused()) 
        m_clients.focused()->drop_input_focus();
    m_clients.focus_front();
    if (has_focused()) {
        m_clients.focused()->take_input_focus();
        m_clients.focused()->raise();
    }
}

void workspace::focused_toggle_floating() {
    if (has_focused()) {
        m_clients.focused()->toggle_floating();
        m_clients.focused()->drop_input_focus();
    }
}

void workspace::move_focus(int i) {
    if (has_focused())
        m_clients.focused()->drop_input_focus();
    m_clients.circulate_focus(i);
    if (has_focused()) {
        m_clients.focused()->take_input_focus();
        m_clients.focused()->raise();
    }
}

void workspace::move_focused(int i) {
    auto& clientPosition = m_clients.focused();
    // due to how splice works in case of i > 0 correction is needed so splice
    // doesn't leave the list unchanged
    auto next_position = m_clients.circulate_next_with_end((i > 0) ? i + 1 : i);
    //( newpos, other, currpos)
    m_clients.splice(next_position, m_clients, clientPosition);
    // rearange new stack
    arrange();
}

void workspace::move_focused_to_top() {
    m_clients.splice(m_clients.begin(), m_clients, m_clients.focused());
    // arrange/display after changing client stack
    arrange();
}

void workspace::move_focused_to_workspace(workspace& other) {
    // check if there is no need to move
    if (this == &other) return;

    // since this method is called only on focused workspaces,
    // moving from this ws will move it to hidden ws, so client shoud 
    // be hidden to
    this->m_clients.focused()->hide(); 
    this->m_clients.focused()->drop_input_focus(); 

    // move client to beggining of the other list
    other.m_clients.splice(other.m_clients.begin(), this->m_clients,
                           this->m_clients.focused());

    // assing moved clients reference to workspace to other workspace
    other.m_clients.begin()->set_parent_workspace(&other);

    // reset iterators to focused clients
    other.m_clients.focus_front();
    this->focus_front(); // triger take_focus on new clien

    // rearange workspaces
    // TODO: maybe have an should_arrange flag so 
    // we can avoid rearanging and hiding here risking flickering clients
    // and avoid easy fix for this flickering with arangin on every show_clients()
    other.arrange(); 
    other.hide_clients();
    this->arrange();
 
    // no need to update bar, since focus_front() will change net active
    // wich whill triger bar update in window_manager::on_property_notify(...)
}

void workspace::kill_focused() {
    if (not has_focused()) return;
    m_clients.focused()->kill();
    // kill will trigger unmap even which will call workspace::remove_client
    // wich will remove it from list & rearange
}

void workspace::remove_client(Window w) {
    // TODO: use map
    m_clients.erase(std::find_if(
        m_clients.begin(), m_clients.end(),
        [&](client& c) { return c.get_xwindow().get() == w; }));
    // NOTE: not using ws focus front since selected client could be
    // the one removed and checking for end or dropInputFocus could cause 
    // and error
    m_clients.focus_front();
    if (has_focused()) // if focused is not end iter
        m_clients.focused()->take_input_focus();
    arrange();
}

void workspace::create_client(Window w) {
    if (has_focused()) 
        m_clients.focused()->drop_input_focus();
    m_clients.emplace_front(w, this);
    // TODO: make util::focus_list take care of reseting focus on list change
    m_clients.focus_front(); 
    m_clients.front().get_xwindow().mapRaised();
    m_clients.front().take_input_focus();
    arrange();
}

void workspace::show_clients() {
    for (auto& client : m_clients)
        client.show();
    // drop input focus for focused client since
    // it makes no sence to hide workspace and let it keep focus
    if (has_focused()) m_clients.focused()->take_input_focus();
}

void workspace::hide_clients() {
    for (auto& client : m_clients)
        client.hide();
    // return input focus to focused client
    if (has_focused()) m_clients.focused()->drop_input_focus();
}

void workspace::arrange() {
    // TODO: see if it's possible to avoid calcualting ws area
    auto a = m_parent_monitor->rect();
    a.top_left.y += bar::conf.font.getHeight();
    a.height -= bar::conf.font.getHeight();
    m_layout(m_clients, conf.layout, a);
}

void workspace::set_focused_client(client* cl) {
    auto pos = std::find_if(m_clients.begin(), m_clients.end(),
                            [&](const client& c) { return &c == cl; });
    if (pos != m_clients.end()) m_clients.set_focus(pos);
}

void workspace::unfocus() {
    if (has_focused()) m_clients.focused()->drop_input_focus();
}

void workspace::focus() {
    if (has_focused()) m_clients.focused()->take_input_focus();
}

bool workspace::has_focused() { return m_clients.end() != m_clients.focused(); }

}
