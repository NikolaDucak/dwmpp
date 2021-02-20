#pragma once

#include "util/focus_list.h"
#include "wm/client.h"
#include "wm/layout.h"

#include <X11/X.h>

namespace wm {

class monitor;

class workspace {
public:
    struct config {
        //TODO: consider moving vector of workspace names in monitor config
        //since monitor relies on knowing how many workspaces there are
        std::vector<std::string> workspaces;
        layout_config layout;
    };

    /**
     * Default configuration specified in `config.h`, copied 
     * in @ref local_config on workspace creation.
     */
    static const config default_config;

public:
    workspace(monitor* parent_monitor, unsigned index);

    /* 
     * wm::client has a pointer to a workspace it belongs to, so
     * copying/moving is not allowed since it may break
     * clients workspace referance.
     */
    workspace(workspace& m)  = delete;
    workspace(workspace&& m) = delete;
    workspace& operator=(workspace& m) = delete;
    workspace& operator=(workspace&& m) = delete;

    /**
     * Sets funciton thats going to be used to arrange clients
     * on the workspace area.
     */
    void set_layout(layout_function l);

    /**
     * If there is a focused client toggles floating on him and 
     * calls wm::workspace::arange
     */
    void focused_toggle_floating();

    /**
     * Resets clients focus first client int the list,
     * drops input focus of the first client and gives it to 
     * the first client.
     */
    void focus_front();

    /**
     * Moves focus to next @p i client relative to the currently focused 
     * cleint, droping input focus from currently focused client and giving it
     * to the new client.
     */
    void move_focus(int i);

    /**
     * Moves focused client by @p i places on on the list relative to 
     * it's current place and calls layout function to rearange clients.
     */
    void move_focused(int i);

    /**
     * Moves focused client to the first place in the list of clients then
     * calls layout funciton to rearange clients.
     */
    void move_focused_to_top();

    /**
     * Just straight up move all clients from @p other workspace 
     * to this one, leaving other empty.
     */
    void take_clients(workspace& other);

    /**
     * Uses std::list<T>::splice to move focused client to the top
     * of the client list of the @p other_ws workspace, then
     * both workspaces are then rearanged with their corresponding layout
     * funcitons.
     */
    void move_focused_to_workspace(workspace& other_ws);

    /**
     *
     */
    void kill_focused();

    /**
     * Checks if client for @p w is found inside `m_clients`, if it is, then it
     * is erased and workspace rearanges itself with call to wm::workspace::arrange();
     */
    void remove_client(Window w);

    /**
     * Creates wm::client and puts it at the front of the `m_clients` list, while
     * checking if @p w should be floating in case of it being a transient window,
     * After that rearanges itself with wm::workspace::arrange();
     */
    void create_client(Window w);

    /**
     * Hides clients and drops input focus from the client marked as focused by
     * focus list.
     */
    void show_clients();

    /**
     * Shows clients and gives input focus to the client marked as focused by
     * focus list.
     */
    void hide_clients();

    /**
     * Using `m_layout` member, arranges & displays `m_clients` on the display.
     */
    void arrange();

    /**
     * Circulates internal focus iterator of `m_clients` until its focusing 
     * client whose address matches @p cl
     */
    void set_focused_client(client* cl);

    /**
     * Drops input focus and changes border color of current focused window
     * but leaves internal client list focused on said client.
     * @note This funciton is used for multi monitor support since
     * Multiple monitors means multiple visible workspaces butt only
     * one focused (taking input).
     */
    void unfocus();

    /**
     * Drops input focus and changes border color of current focused window
     * but leaves internal client list focused on said client.
     * @note This funciton is used for multi monitor support since
     * Multiple monitors means multiple visible workspaces butt only
     * one focused (taking input).
     */
    void focus();

    /**
     * Return true if the current focused client iterator
     * is pointing not pointing at end, such is the case when
     * workspace has no clients.
     */
    bool has_focused();

    [[nodiscard]] auto        empty() const { return m_clients.empty(); }
    [[nodiscard]] auto        get_index() const { return m_index; }
    [[nodiscard]] auto&       clients() { return m_clients; }
    [[nodiscard]] const auto& clients() const { return m_clients; }
    [[nodiscard]] auto&       get_parent_monitor() { return *m_parent_monitor; }

private:
    uint                     m_index;
    layout_function          m_layout;
    monitor*                 m_parent_monitor;
    util::focus_list<client> m_clients;
    /** 
     * Configuration specific to this workspace, allowing
     * different layout configuration for each separate workspace.
     */
    config                   m_local_config;
};

}  // namespace wm
