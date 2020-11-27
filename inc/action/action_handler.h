#pragma once

namespace action {

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

class action_handler {
public:
    virtual void handle_move_focus(move_focus& mv) = 0;
    virtual void handle_move_focused(move_focused& mf) = 0;
    virtual void handle_move_focused_to_top(move_focused_to_top& mt) = 0;
    virtual void handle_move_focused_to_workspace(move_focused_to_workspace& mw) = 0;
    virtual void handle_focus_workspace(focus_workspace& fw) = 0;
    virtual void handle_set_layout(set_layout&) = 0;
    virtual void handle_quit_dwmpp(quit_dwmpp&) = 0;
    virtual void handle_spawn(spawn&) = 0;
    virtual void handle_shell_command(shell_command&) = 0;
    virtual void handle_toggle_floating(toggle_floating&) = 0;
    virtual void handle_kill_focused(kill_focused& k) = 0;
    virtual void handle_toggle_bar(toggle_bar& k) = 0;
};

}  // namespace action
