#pragma once

#include "Client.h"

#include <vector>

class Monitor;

/*
 * std::list::splice(): No elements are copied or moved, only the internal pointers of the list nodes are re-pointed, no iterators are invalidated
 * Transfers the element pointed to by it from other into *this. The element is inserted before the element pointed to by pos.
 */
class Workspace {
public:

	// configuration
	struct Config{
		std::vector< std::string > workspaces;
		int innerGap, outerGap;
		int factor, mastersNum;
	};
	// default config;
	static const Config config;
	// specific config for this workspace, allows for workspace specific factor gap changes
	Config config_;

public:

	Workspace( Monitor& m, unsigned index );
	Workspace( Workspace& m ) = delete;
	Workspace( Workspace&& m ) = delete;
	Workspace& operator = ( Workspace& m ) = delete;
	Workspace& operator = ( Workspace&& m ) = delete;

	// move focus to client in stack
	void moveFocus(int i);
	void focusFront();// restart forcus

	// move selected client in stack by 'i' positions
	void moveSelectedClient(int i);
	void moveSelectedClientToTop();

	// workspace handles fullscreen client instead of client himself
	// so no need to search for fullscreen client
	void toggleFullscreenOnSelectedClient();

	// used for workspace switching
	void showAllClients();
	void hideAllClients();

	// creating clients & destroying them
	void addClient( Window w );
	void removeClient( Client& c );
	void removeClient( Window w );

	// master
	void resizeMaster( int i );


	// TODO: pass arranger
	/*
	 * Arranges clients in rectangular area with top left point 'topleft'
	 * and with width & height specified in point dim,
	 * gaps will be applied inside specified area
	 */
	void arrangeClients( int barHeight, point dim );
	void moveSelectedClientToWorkspace( Workspace& ws );

	// getters
	inline Monitor& getMonitor()            { return *monitorRef_; }
	inline uint getIndex() const            { return index_; }
	inline List<Client>& getClients()       { return clients_; }
	inline Client& getSelectedClient()      { return *selectedClientIter_; }
	inline Config& getConfig()              { return config_; };

	inline bool hasSelectedClient() const   { return selectedClientIter_ != clients_.end(); }
	inline Client* getFullscreenClient() const { return fullscreenClient_; }
	void setSelectedClient(Client& c);


private:

	// arranger
	void arrange( int barHeight, int screenW, int screenH);
	
	// helper method to treat regular iterator like cirulator
	using ClientListIterator = typename std::list<Client>::iterator;
	ClientListIterator circulate( std::list<Client>& list, ClientListIterator& curr, int i );
	ClientListIterator circulateWithEndIterator( std::list<Client>& list, ClientListIterator& curr, int i );
	
	Monitor* monitorRef_;
	uint index_;
	List<Client> clients_;
	ClientListIterator selectedClientIter_;	
	Client* fullscreenClient_;
};
