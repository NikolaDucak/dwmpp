#pragma once

#include "Client.h"
#include "util/point.h"
#include "util/focus_list.h"
#include <vector>
#include <list>

class Monitor;

/*
 * std::list::splice(): No elements are copied or moved, only the internal pointers of the list nodes are re-pointed, no iterators are invalidated
 * Transfers the element pointed to by it from other into *this. The element is inserted before the element pointed to by pos.
 */
class Workspace {
public:
    // configuration
    struct Config {
        std::vector<std::string> workspaces;
        int innerGap, outerGap;
        int factor, mastersNum;
    };
    // default config;
	static const Config config;
	// specific config for this workspace, allows for 
    // workspace specific factor, gap etc changes

public:

	Workspace(Monitor& m, unsigned index);
	Workspace(Workspace& m) = delete;
	Workspace(Workspace&& m) = delete;
    Workspace& operator=(Workspace& m) = delete;
    Workspace& operator=(Workspace&& m) = delete;

    // move focus to client in stack
	void moveFocus(int i);
	void focusFront();// restart forcus

	// move selected client in stack by 'i' positions
	void moveFocusedClient(int i);
	void moveFocusedClientToTop();

	// workspace handles fullscreen client instead of client himself
	// so no need to search for fullscreen client
	void toggleFullscreenOnSelectedClient();

	// used for workspace switching
	void showAllClients();
	void hideAllClients();

	// creating clients & destroying them
	void createClientForWindow(Window w);
	void removeClientForWindow(Window w);
	void removeClient(Client& c);

	// master
	void resizeMaster(int i);

    // 

	// TODO: pass arranger
	/*
	 * Arranges clients in rectangular area with top left point 'topleft'
	 * and with width & height specified in point dim,
	 * gaps will be applied inside specified area
	 */
	void arrangeClients(int barHeight, point dim);
	void moveSelectedClientToWorkspace(Workspace& ws);

	// getters
	inline Monitor& getMonitor()            { return *m_monitorPtr; }
	inline uint getIndex() const            { return index_; }
	inline util::focus_list<Client>& getClients()  { return m_clients; }
	inline Client& getSelectedClient()      { return *m_clients.focused(); }
	inline Config& getConfig()              { return m_config; };

	inline bool hasSelectedClient() const   { return m_clients.focused() != m_clients.end(); }
	inline Client* getFullscreenClient() const { return fullscreenClient_; }
	void setSelectedClient(Client& c);

private:

	// arranger
	void arrange(int barHeight, int screenW, int screenH);
	
	Monitor* m_monitorPtr;
	uint index_;
    util::focus_list<Client> m_clients;
	Client* fullscreenClient_;
	Config m_config;
};
