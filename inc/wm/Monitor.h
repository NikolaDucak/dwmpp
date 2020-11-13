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

    void selectWorkspace(unsigned i);
    void updateBar();
	inline Bar& getBar()     { return this->m_bar; }

	inline std::array<Workspace, 10>& getWorkspaces()  { return m_workspaces; }
	inline Workspace& getSelectedWorkspace()           { return m_workspaces[m_selectedWorkspaceIndex]; }
	inline Client& getSelectedClient()                 { return m_workspaces[m_selectedWorkspaceIndex].getSelectedClient(); }
	inline bool    hasSelectedClient()                 { return m_workspaces[m_selectedWorkspaceIndex].hasSelectedClient(); }
	inline int getSelectedWorkspaceIndex()             { return m_selectedWorkspaceIndex; }
	inline const point& getSize()                      { return wh_; }
	inline const point& getWinPos()  { return win_xy_; }
	inline const point& getWinSize() { return win_xy_; }

private:

	int num;
	point xy_, wh_, win_xy_, win_wh_;

	std::array<Workspace,10> m_workspaces;
	int m_selectedWorkspaceIndex;
	Bar m_bar;
};

