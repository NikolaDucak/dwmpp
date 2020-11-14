#pragma once

#include "xlib/XGraphics.h"
#include "xlib/XWindow.h"
#include "xlib/Xlib.h"
#include "wm/Workspace.h"

#include <iostream>

class Bar {
public:

    struct Config {
        xlib::XColor barBG, barFG;
        xlib::XColor selectedTagBG, selectedTagFG;
        xlib::XColor usedTagBG, usedTagFG;
        xlib::XColor emptyTagBG, emptyTagFG;
        xlib::XFont font;
    };

    /*
     * Configuration for all bars.
     */
    static Config config;

public:

    Bar();

    void setStatusString(const std::string& status) { m_status = status; }
    void setTitleString(const std::string& title)   { m_title = title; }

    //void draw(const util::focus_list<Workspace> workspaces, uint selectedWSIndex);
    void draw(const std::array<Workspace,10>& workspaces, uint selectedWSIndex);

    std::string& getStatusString();
    std::string& getTitleString();

    // Methods that call xlib wrapper to get atom properites
    void updateTitle(const std::string& title) { setTitleString(title); }
    void updateStatus(const std::string& status);

private:

    bool m_hidden;
    int m_width;
    xlib::XWindow m_xwin;
    std::string m_status;
    std::string m_title;
};
