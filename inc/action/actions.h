#pragma once 

#include <string>
#include <iostream>

#include "wm/layout.h"
#include "action/action_base.h"
#include "action/action_handler.h"

namespace action {

struct move_focus : public action_base {
    move_focus(int i) : i(i) {}
    inline virtual void execute(action_handler& handler) override 
        { handler.handle_move_focus(*this); }
    int i;
};

struct move_focused : public action_base {
    inline virtual void execute(action_handler& handler) override 
        { handler.handle_move_focused(*this); }
    int i;
};

struct move_focused_to_top : public action_base {
    inline virtual void execute(action_handler& handler) override 
        { handler.handle_move_focused_to_top(*this); }
};

struct move_focused_to_workspace : public action_base {
    move_focused_to_workspace(unsigned i) : i(i) {}
    inline virtual void execute(action_handler& handler) override 
        { handler.handle_move_focused_to_workspace(*this); }
    unsigned i;
};

struct focus_workspace : public action_base {
    focus_workspace(unsigned i) : i (i) {}
    inline virtual void execute(action_handler& handler) override 
        { handler.handle_focus_workspace(*this); }
    unsigned i;
};

struct spawn : public action_base {
    spawn(const std::string& str) : str(str) {}
    spawn(const char* str) : str(str) {}
    inline virtual void execute(action_handler& handler) override 
        { handler.handle_spawn(*this); }
    std::string str;
};

struct set_layout : public action_base {
    set_layout(wm::layout_function l) : layout(l) {}
    inline virtual void execute(action_handler& handler) override 
        { handler.handle_set_layout(*this); }
    wm::layout_function layout;
};

struct quit_dwmpp: public action_base {
    inline virtual void execute(action_handler& handler) override 
        { handler.handle_quit_dwmpp(*this); }
};

struct shell_command : public action_base {
    inline virtual void execute(action_handler& handler) override 
        { handler.handle_shell_command(*this); }
};

struct toggle_floating : public action_base {
    inline virtual void execute(action_handler& handler) override 
        { handler.handle_toggle_floating(*this); }
};

struct kill_focused : public action_base {
    inline virtual void execute(action_handler& handler) override 
        { handler.handle_kill_focused(*this); }
};

struct toggle_bar : public action_base {
    inline virtual void execute(action_handler& handler) override 
        { handler.handle_toggle_bar(*this); }
};

}  // namespace action
