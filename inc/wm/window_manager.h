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
#include "wm/layout.h"

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
    ~window_manager();

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
    inline virtual void handle_kill_focused(action::kill_focused& /* unused */) override
        { m_monitors.focused()->workspaces().focused()->kill_focused(); }

    /** */
    inline virtual void handle_toggle_floating(action::toggle_floating&) override 
        { m_monitors.focused()->workspaces().focused()->focused_toggle_floating(); }

    /** */
    inline virtual void handle_toggle_bar(action::toggle_bar& /* unused */)  override
        { m_monitors.focused()->bar().toggle_visibility(); m_monitors.focused()->workspaces().focused()->arrange(); }

    /** */
    inline virtual void handle_set_layout(action::set_layout& l) override 
        { m_monitors.focused()->workspaces().focused()->set_layout(l.layout); }

    /** */
    inline virtual void handle_quit_dwmpp(action::quit_dwmpp&) override 
        { LOG("WM: QUIT"); m_running = false; }

    /** */
    inline virtual void handle_spawn(action::spawn& s) override 
        { LOG("WM: SPAWN"); util::spawn_process(s.str); }

    /** */
    inline virtual void handle_shell_command(action::shell_command&) override 
        { LOG("SHELL CMD - UNIMPLEMENTED" ); }

    /** */
    virtual void handle_layout_update(action::layout_update<layout_config>& lyupdate) override {
        m_monitors.focused()->workspaces().focused()->updateLayout(lyupdate.impl);

    }

private: // x event handlers

    /**
     * Action triggered whenever mouse pointer is moved.
     * This method checks that movement sets cursour outside
     * the current screen. If cursor is now outside the current screen
     * and we find that cursor coords are now inside the other monitor
     * we focus that monitor and that
     * @note dwmpp uses this event only for multimonitor purposes
     */
	void on_motion_notify     (const XMotionEvent& e);       

    /**
     * Action triggered when any x window changes any of its atom properities.
     * This method is conserned only with:
     *      - WM_NAME on root window
     *      - NET_ACTIVE_WINDOW on root window
     *      - WM_TRANSIENT WM_NORMAL_HINT WM_HINTS WM_NAME on any client window
     */
	void on_property_notify  (const XPropertyEvent& e);

    /**
     * Catching changes in keyboard mapping and regrabbing
     * keybindings. Changing keyboard maping breaks keybindings
     * so regrabbing is neccesary.
     */
	void on_mapping_notify    (XMappingEvent& e);

    /**
     * Notification received when client no longer wants to be 
     * drawn on the screen. This can happen when client processes
     * is finished and needs to be removed from the workspace
     */
	void on_unmap_notify      (const XUnmapEvent& e);

    /**
     * Event used for updating changes in number of physical monitor.
     * Change in physical monitors is manifested through change in root 
     * window size. Method checks only if root was reconfigured and 
     * updates m_monitors method.
     * @note dwmpp uses this event only for detecting changes physical
     * monitor number or layout.
     */
	void on_configure_notify  (const XConfigureEvent& e);

    /** 
     * Event triggered when process using that window has ended. 
     */
	void on_destroy_notify    (const XDestroyWindowEvent& e);

    /**
     * XCrossingEvent is either LeaveNotify or EnterNotify.
     * These events occur when the pointer enters or leaves a window.
     * These events are selected with XEnterWindowMask and XLeaveWindowMask 
     * with XSelectInput.
     */
	void on_enter_notify      (const XCrossingEvent& e);

    /**
     * A ConfigureRequest event reports when another client attempts to 
     * change a window’s size, position, border, and/or stacking order.
     * This event differs from ConfigureNotify in that it delivers the 
     * parameters of the request before it is carried out. This gives 
     * the client that selects this event (usually the window manager) 
     * the opportunity to revise the requested configuration before 
     * executing the XConfigureWindow() request itself or to deny the 
     * request. (ConfigureNotify indicates the final outcome of the
     * request.)
     */
	void on_configure_request (const XConfigureRequestEvent& e);

    /**
     * A MapRequest event occurs when the functions XMapRaised() 
     * and XMapWindow() are called (when client wants to be drawn 
     * on the screen.  This event differs from MapNotify in that 
     * it delivers the parameters of the request before it is carried 
     * out. This gives the client that selects this event (usually 
     * the window manager) the opportunity to revise the size or position 
     * of the window before executing the map request itself or to deny 
     * the request. (MapNotify indicates the final outcome of the request.)
     */
	void on_map_request       (const XMapRequestEvent& e);

    /** 
     * Event sent to WM when user presses one of the grabed keys 
     * (grabbed keys are config keybindings). 
     */
	void on_key_press         (const XKeyEvent& e);

    /**
     * Event sent to WM when user klicks one of the mouse buttons 
     * from configuration. Buttons p
     */
	void on_button_press      (const XButtonPressedEvent& e);
    
    /**
     * A ClientMessage event is sent as a result of a call to 
     * XSendEvent() by a client to a particular window. 
     * Any type of event can be sent with XSendEvent().
     * DWMPP uses this to check if client wants to bee fullscreen
     * or requires attention (urgent).
     */
	void on_client_message    (const XClientMessageEvent& e);

    /**
     * An Expose event is generated when a window becomes visible or a 
     * previously invisible part of a window becomes visible.
     */
	void on_expose            (const XExposeEvent& e);

    /**
     * FocusIn and FocusOut events occur when the keyboard focus window changes
     * as a result of an XSetInputFocus() call. They are much like EnterNotify 
     * and LeaveNotify events except that they track the focus rather than 
     * the pointer.
     */
	void on_focus_in          (const XFocusChangeEvent& e);

// clang-format on

private: // utility methods
    void grab_keys();
    void focus_monitor(monitor* m);

    client*  get_client_for_window(Window w);
    monitor* get_monitor_for_rectangle(const util::rect& rect);
    monitor* get_monitor_for_pointer_coords(int x, int y);
    monitor* get_monitor_for_window(Window w);
    client&  get_focused_client();

private:
    bool m_running;
    util::focus_list<monitor> m_monitors;
    xlib::XCore* m_x;
};

}  // namespace wm
