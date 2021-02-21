#pragma once

#include "wm/layout.h"

namespace action {

/*
 * All keybindding actions that dwmpp supports
 */

struct move_focus;
struct move_focused;
struct move_focused_to_top;
struct move_focused_to_workspace;
struct focus_workspace;
struct spawn;
struct set_layout;
struct quit_dwmpp;
struct shell_command;
struct toggle_floating;
struct kill_focused;
struct toggle_bar;

template<typename T>
struct layout_update;

class action_handler {
public:
    virtual void handle_move_focus(move_focus&) = 0;
    virtual void handle_move_focused(move_focused&) = 0;
    virtual void handle_move_focused_to_top(move_focused_to_top&) = 0;
    virtual void handle_move_focused_to_workspace(move_focused_to_workspace&) = 0;
    virtual void handle_focus_workspace(focus_workspace&) = 0;
    virtual void handle_set_layout(set_layout&) = 0;
    virtual void handle_quit_dwmpp(quit_dwmpp&) = 0;
    virtual void handle_spawn(spawn&) = 0;
    virtual void handle_shell_command(shell_command&) = 0;
    virtual void handle_toggle_floating(toggle_floating&) = 0;
    virtual void handle_kill_focused(kill_focused&) = 0;
    virtual void handle_toggle_bar(toggle_bar&) = 0;
    virtual void handle_layout_update(layout_update<wm::layout_config>&) = 0;
};

}  // namespace action
