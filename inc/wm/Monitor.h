#pragma once 

#include <list>
#include <array>
#include "util/point.h"

#include "Bar.h"
#include "Workspace.h"

/*
 * Workspaces have pointer references to Monitor they belong to
 * moving a monitor leaves pointers pointing to old adress
 */ 
class Monitor {
public:
    static void updateMonitors(std::list<Monitor>& currentMonitors);

    Monitor(int num, point xy, point wh, point window_xy, point window_wh);
    Monitor(Monitor&)  = delete;
    Monitor(Monitor&&) = delete;
    Monitor& operator=(Monitor&) = delete;
    Monitor& operator=(Monitor&&) = delete;

    inline void selectWorkspace(unsigned i) {
        //TODO: custom number of workspaces
        if (i < Workspace::config.workspaces.size()) {
            getSelectedWorkspace().hideAllClients();
            selectedWorkspaceIndex_ = i;
            getSelectedWorkspace().showAllClients();
        }
        
    }
	//inline void updateBar()  { bar_.draw( workspaces_, selectedWorkspaceIndex_ );  }
	inline Bar& getBar()     { return this->m_bar; }

	inline std::array<Workspace, 10>& getWorkspaces()  { return workspaces_; }
	inline Workspace& getSelectedWorkspace()           { return workspaces_[selectedWorkspaceIndex_]; }
	inline Client& getSelectedClient()                 { return workspaces_[selectedWorkspaceIndex_].getSelectedClient(); }
	inline bool    hasSelectedClient()                 { return workspaces_[selectedWorkspaceIndex_].hasSelectedClient(); }
	inline int getSelectedWorkspaceIndex()             { return selectedWorkspaceIndex_; }
	inline const point& getSize()                      { return wh_; }
	inline const point& getWinPos()  { return win_xy_; }
	inline const point& getWinSize() { return win_xy_; }

private:

	int num;
	point xy_, wh_, win_xy_, win_wh_;

	std::array<Workspace,10> workspaces_;
	int selectedWorkspaceIndex_;
	Bar m_bar;
};

