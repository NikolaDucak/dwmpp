#pragma once 

#include <vector>
#include <memory>

#include "util/spawn_process.h"
#include "xlib/XCore.h"
#include "util/focus_list.h"
#include "wm/monitor.h"
#include "action/action_base.h"
#include "action/action_handler.h"
#include "action/actions.h"

#ifdef DEBUG_LOG
#    include <iostream>
#    define LOG(x) std::cout << x << std::endl;
#else 
#    define LOG(x)
#endif

namespace wm {

struct keybinding {
	int mod;
	KeySym keySym;
    action::action_base* action;


    ~keybinding() { 
        // NOTE: config uses initializer lists and that
        // makes vector of keybindings copy constructs
        // deleting temporary keybindings, deleteing aciton
        // delete action; 
    }
};

class window_manager : public action::action_handler {
public:  // static variables
    struct config {
        std::vector<keybinding> keybindings;
    };

    static const config conf;

public:
    window_manager(xlib::XCore* x);

    void run();

// clang-format off
    /** */
    inline virtual void handle_move_focus(action::move_focus& mv) override  
        { m_monitors.focused()->workspaces().focused()->move_focus(mv.i); }

    /** */
    inline virtual void handle_move_focused(action::move_focused& mv) override
        { m_monitors.focused()->workspaces().focused()->move_focused(mv.i); }

    /** */
    inline virtual void handle_move_focused_to_top(action::move_focused_to_top& /*unused*/) override
        { m_monitors.focused()->workspaces().focused()->move_focused_to_top(); }

    /** */
    inline virtual void handle_move_focused_to_workspace(action::move_focused_to_workspace& ws) override { 
        m_monitors.focused()->workspaces().focused()->move_focused_to_workspace( 
                m_monitors.focused()->get_workspace(ws.i) ); 
    }

    /** */
    inline virtual void handle_focus_workspace(action::focus_workspace& fw)  override
        { m_monitors.focused()->focus_workspace(fw.i); }
    
    /** */
    inline virtual void handle_kill_focused(action::kill_focused& /*unused*/) override
        { m_monitors.focused()->workspaces().focused()->kill_focused(); }

    /** */
    inline virtual void handle_toggle_floating(action::toggle_floating&) override 
        { m_monitors.focused()->workspaces().focused()->focused_toggle_floating(); }

    /** */
    inline virtual void handle_toggle_bar(action::toggle_bar& /*unused*/)  override
        { m_monitors.focused()->bar().toggle_visibility(); }

    /** */
    inline virtual void handle_set_layout(action::set_layout& l) override 
        { m_monitors.focused()->workspaces().focused()->set_layout(l.layout); }

    /** */
    inline virtual void handle_quit_dwmpp(action::quit_dwmpp&) override 
        { LOG("WM: QUIT") m_running = false; }

    /** */
    inline virtual void handle_spawn(action::spawn& s) override 
        { LOG("WM: SPAWN"); util::spawn_process(s.str); }

    /** */
    inline virtual void handle_shell_command(action::shell_command&) override 
        { LOG("SHELL CMD - UNIMPLEMENTED" ); }

private: // x event handlers
    /** Method called when user moves the crusor. */
	void on_motion_notify     (const XMotionEvent& e);       

    /** Action trigeret when atom properity of window is changed. */
	void on_properity_notify  (const XPropertyEvent& e);

    /** Event trigered on changing keyboard mapping. */
	void on_mapping_notify    (XMappingEvent& e);

    /** Event triggered when a client doesn't want to be drawn. */
	void on_unmap_notify      (const XUnmapEvent& e);

    /** Event triggered when client is reconfigured (size, position etc.).*/
	void on_configure_notify  (const XConfigureEvent& e);

    /** Event triggered when process using that window has ended. */
	void on_destroy_notify    (const XDestroyWindowEvent& e);

    /** Event triggered when mouse cursor enters client area. */
	void on_enter_notify      (const XCrossingEvent& e);

    /** */
	void on_configure_request (const XConfigureRequestEvent& e);

    /** Event sent to WM when a client wants to be drawn on the screen. */
	void on_map_request       (const XMapRequestEvent& e);

    /** Event sent to WM when user presses one of the grabed keys (grabbed keys are config keybindings). */
	void on_key_press         (const XKeyEvent& e);

    /** Event sent to WM when user klicks one of the mouse buttons from configuration. */
	void on_button_press      (const XButtonPressedEvent& e);
    
    /** */
	void on_client_message    (const XClientMessageEvent& e);

    /** Event sent to WM whenever an obstructed part of client is visible again. */
	void on_expose            (const XExposeEvent& e);

    /** Event sent to WM whenever a client takes input focus. */
	void on_focus_in          (const XFocusChangeEvent& e);

// clang-format on

private: // utility methods
    void grab_keys();
    client*  get_client_for_window(Window w);
    monitor* get_monitor_for_pointer_coords(int x, int y);
    monitor* get_monitor_for_window(Window w);
    void     focus_monitor(monitor* m);


private:
    bool m_running;
    util::focus_list<monitor> m_monitors;
    xlib::XCore* m_x;
};

}  // namespace wm
