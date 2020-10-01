#pragma once

#include <string>
#include <array>

#include "XLibWrapper.h"
#include "Workspace.h"

class Bar {
public:

	struct Config{
		Color barBG, barFG;
		Color selectedBarBG, selectedBarFG;
		Color usedTagBG, usedTagFG;
		Color emptyTagBG, emptyTagFG;
		XFont font;
	};

	static const Config config;

public:

	Bar( int w );
	void highlightWorkspace(int i);
	void draw( std::array<Workspace,10>& workspaces , uint selectedWorkspaceIndex );

	inline void setWidth( int w ) { width_ = w; };
	inline void setTitle( const std::string& title )      { titleString_ = title; }
	inline void setStatusString( const std::string& str ) { statusString_ = str; }
	inline void setHidden( bool h ) { hidden_ = h; }

	inline const Window& getXWindow() const { return win_; }
	inline uint getHeight() const  {  return config.font.getHeight(); }
	inline uint isHidden() const  {  return hidden_; }

	void toggle();

private:
	bool hidden_ = false;
	Window win_;
	int width_;
	std::string titleString_, statusString_;

};
