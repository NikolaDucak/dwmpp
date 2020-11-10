#pragma once

#include "config/ConfigTypes.h"
#include "xlib/XWindow.h"
#include "xlib/XGraphics.h"
#include "xlib/Xlib.h"

class Bar {
public:
    static void configure(const config::BarConfig& conf);

	struct Config {
        xlib::XColor barBG, barFG;
        xlib::XColor selectedBarBG, selectedBarFG;
        xlib::XColor usedTagBG, usedTagFG;
        xlib::XColor emptyTagBG, emptyTagFG;
        xlib::XFont  font;
	};
    static Config config;

    /*
     * Configuration for all bars.
     */
public:
    Bar();

    void setStatusString(const std::string& status)
    { m_status = status; }
    void setTitleString(const std::string& title)
    { m_title = title; }
    void redraw();
    void update() { redraw(); }

    std::string& getStatusString();
    std::string& getTitleString();

    // Methods that call xlib wrapper to get atom properites
    void updateTitle(const std::string& title) {
        setTitleString(title);
    }
    void updateStatus();

private:

    bool m_hidden;
    int m_width;
    xlib::XWindow m_xwin;
    std::string m_status;
    std::string m_title;
};
