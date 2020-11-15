#include "wm/Workspace.h"

#include "config/Config.h"

void Workspace::arrange(int barHeight, int screenW, int screenH) {
    // TODO: use configure(..) static method
    if (fullscreenClient_) {
        fullscreenClient_->raise();
        fullscreenClient_->resize(point { 0, barHeight },
                                  point { screenW, screenH - barHeight });
        return;
    }
    auto tiledClientsNum =
        std::count_if(m_clients.begin(), m_clients.end(),
                      [](const Client& c) { return not c.isFloating(); });

    if (tiledClientsNum == 1) {
        m_clients.front().resize(
            point { m_config.outerGap, m_config.outerGap + barHeight },
            point { screenW, screenH - barHeight } - m_config.outerGap * 2);
    } else {
        int slavesNum = tiledClientsNum - m_config.mastersNum;
        int masterW   = screenW * m_config.factor * 0.01f;
        int masterH =
            ((screenH - barHeight) - m_config.outerGap) / m_config.mastersNum -
            m_config.innerGap;
        int masterCurrentY = barHeight + m_config.innerGap;

        int slaveX = m_config.outerGap + masterW + m_config.innerGap;
        int slaveW = screenW - slaveX - m_config.outerGap;
        int slaveH = ((screenH - barHeight) - m_config.outerGap) / slavesNum -
                     m_config.innerGap;
        int currentY = barHeight + m_config.outerGap;

        int currentYinc = slaveH + m_config.innerGap;
        int masterYInc  = masterH + m_config.innerGap;

        int arrangedMasters = 0;
        for (auto& c : m_clients) {
            if (c.isFloating()) {
                c.raise();
            } else if (arrangedMasters < m_config.mastersNum) {
                c.resize(point { m_config.outerGap, masterCurrentY },
                         point { masterW, masterH });
                masterCurrentY += masterYInc;
                arrangedMasters++;
            } else {
                c.resize(point { slaveX, currentY }, point { slaveW, slaveH });
                currentY += currentYinc;
            }
        }
    }
}

Workspace::Workspace(Monitor& m, uint index) :
    m_monitorPtr(&m), index_(index), m_clients(), fullscreenClient_(nullptr),
    m_config(config)  // defualt config
{}

void Workspace::focusFront() {
    // drop focus drop active atom and change window border
    if (hasSelectedClient())
        m_clients.focused()->dropInputFocus();
    // get new focused client
    m_clients.focus_front();
    // give focus set active atom and change window border
    if (hasSelectedClient())
        m_clients.focused()->takeInputFocus();
}

void Workspace::toggleFullscreenOnSelectedClient() {
    if (fullscreenClient_) {
        fullscreenClient_->setFullscreen(false);
        fullscreenClient_ = nullptr;
    } else {
        fullscreenClient_ = &*m_clients.focused();
        fullscreenClient_->setFullscreen(true);
    }
    //NOTE: don't rearange all clients, let client resize it self
}

void Workspace::moveFocus(int i) {
    // drop focus drop active atom and change window border
    if (hasSelectedClient())
        m_clients.focused()->dropInputFocus();
    // get new focused client
    m_clients.circulate_focus(i);
    // give focus set active atom and change window border
    if (hasSelectedClient())
        m_clients.focused()->takeInputFocus();
}

void Workspace::showAllClients() {
    for (auto& client : m_clients)
        client.show();
    if (hasSelectedClient())
        m_clients.focused()->takeInputFocus();
}

void Workspace::hideAllClients() {
    // hide all clients from visible surface
    for (auto& client : m_clients)
        client.hide();
    // remove input focus from selected client
    if (hasSelectedClient())
        m_clients.focused()->dropInputFocus();
}

void Workspace::arrangeClients() {
    arrange(Bar::config.font.getHeight(),
            m_monitorPtr->getSize().x,
            m_monitorPtr->getSize().y);
}

void Workspace::createClientForWindow(Window w) {
    // emplacing does not invalidate iterators
    m_clients.emplace_front(*this, w);
    m_clients.front().getXWindow().mapRaised();
}

void Workspace::removeClientForWindow(Window w) {
    // NOTE: not using std::remove_if since remove if relies on Client class
    // having a move operator
    m_clients.erase(std::find_if(
        m_clients.begin(), m_clients.end(),
        [&](const Client& c) { return c.getXWindow().get() == w; }));
    // NOTE: not using ws focus front since selected client could be
    // the one removed and checking for end or dropInputFocus could cause 
    // and error
    m_clients.focus_front();
    if (hasSelectedClient()) // if focused is not end iter
        m_clients.focused()->takeInputFocus();
}

void Workspace::removeClient(Client& w) {
    removeClientForWindow(w.getXWindow().get());
}

void Workspace::setSelectedClient(Client& ) {
    /*
    for (auto i = m_clients.begin(); i != m_clients.end(); i++) {
        if (&c == &(*i))
            selectedClientIter_ = i;
    }
    */
}

void Workspace::moveFocusedClient(int i) {
    auto& clientPosition = m_clients.focused();
    // due to how splice works in case of i > 0 correction is needed so splice
    // doesn't leave the list unchanged
    auto nextPosition = m_clients.circulate_next_with_end((i > 0) ? i + 1 : i);
    //( newpos, other, currpos)
    m_clients.splice(nextPosition, m_clients, clientPosition);
}

void Workspace::moveFocusedClientToTop() {
    m_clients.splice(m_clients.begin(), m_clients, m_clients.focused());
}

void Workspace::moveSelectedClientToWorkspace(Workspace& other) {
    // move client to beggining of the other list
    other.m_clients.splice(other.m_clients.begin(), this->m_clients,
                           this->m_clients.focused());
    // assing moved clients reference to workspace to other workspace
    other.m_clients.begin()->assignToWorkspace(other);
    // reset iterators to focused clients
    other.m_clients.focus_front();
    this->m_clients.focus_front();
}

