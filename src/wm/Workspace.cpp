#include "wm/Workspace.h"
#include "config/Config.h"

using ClientListIterator = typename std::list<Client>::iterator;

void Workspace::configure(const config::WorkspaceConfig&) {
}

void Workspace::arrange( int barHeight, int screenW, int screenH ) {
    //TODO: use configure(..) static method
    auto& config = config::workspace;
    if (fullscreenClient_) {
        fullscreenClient_->resize(point { 0, barHeight },
                                  point { screenW, screenH - barHeight });
        return;
    }
    auto tiledClientsNum =
        std::count_if(m_clients.begin(), m_clients.end(),
                      [](const Client& c) { return not c.isFloating(); });

    if (tiledClientsNum == 1) {
        m_clients.front().resize(
            point { config.outerGap, config.outerGap + barHeight },
            point { screenW, screenH - barHeight } - config.outerGap * 2);
    } else {
        int slavesNum = tiledClientsNum - config.mastersNum;
        int masterW   = screenW * config.factor * 0.01;
        int masterH =
            ((screenH - barHeight) - config.outerGap) / config.mastersNum -
            config.innerGap;
        int masterCurrentY = barHeight + config.innerGap;

        int slaveX = config.outerGap + masterW + config.innerGap;
        int slaveW = screenW - slaveX - config.outerGap;
        int slaveH = ((screenH - barHeight) - config.outerGap) / slavesNum -
                     config.innerGap;
        int currentY = barHeight + config.outerGap;

        int currentYinc = slaveH + config.innerGap;
        int masterYInc  = masterH + config.innerGap;

        int arrangedMasters = 0;
        for (auto& c : m_clients) {
            if (c.isFloating()) {
                c.raise();
            } else if (arrangedMasters < config.mastersNum) {
                c.resize(point { config.outerGap, masterCurrentY },
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

ClientListIterator Workspace::circulate( std::list<Client>& list, ClientListIterator& curr, int i ){
	// assume its not already on the end
	auto temp = curr;
	if( i > 0 ){
		while( i-- ){
			temp++;
			if( temp == list.end() )
				temp = list.begin();
		}
	}
	else if( i < 0 ){
		while( i++ ){
			if( temp == list.begin() || temp == list.end() )
				temp = --list.end();
			else
				temp--;
		}
	}
	return temp;
}

ClientListIterator Workspace::circulateWithEndIterator( std::list<Client>& list, ClientListIterator& curr, int i){
	auto temp = curr;
	if( i > 0 ){
		while( i-- > 0 ){
			if( temp == list.end() )
				temp = list.begin();
			else
				temp++;
		}
	}
	else if( i < 0 ) {
		while( i++ < 0 ){
			if( temp == list.begin() )
				temp = list.end();
			else
				temp--;
		}
	}
	return temp;
}

Workspace::Workspace(Monitor& m, uint index) :
    // config_( config ), // defualt config
    m_monitorPtr(&m), index_(index), m_clients(),
    selectedClientIter_(m_clients.begin()),  // TODO: is this safe?
    fullscreenClient_(nullptr) {}

void Workspace::focusFront() {
    // drop focus drop active atom and change window border
    if (selectedClientIter_ != m_clients.end())
        selectedClientIter_->dropInputFocus();
    // get new focused client
    selectedClientIter_ = m_clients.begin();
    // give focus set active atom and change window border
    if (selectedClientIter_ != m_clients.end())
        selectedClientIter_->takeInputFocus();
}

void Workspace::toggleFullscreenOnSelectedClient() { 
	fullscreenClient_ = (fullscreenClient_) ? nullptr : & *selectedClientIter_; 
}

void Workspace::moveFocus(int i) {
    // drop focus drop active atom and change window border
    if(selectedClientIter_ != m_clients.end() )
        selectedClientIter_->dropInputFocus();
    // get new focused client
    selectedClientIter_ = circulate(m_clients, selectedClientIter_, i);
    // give focus set active atom and change window border
    selectedClientIter_->takeInputFocus();
}

void Workspace::showAllClients() {
    for (auto& client : m_clients)
        client.show();
}

void Workspace::hideAllClients() {
    for (auto& client : m_clients)
        client.hide();
}

void Workspace::arrangeClients(int barHeight, point wh) {
    arrange(barHeight, wh.x, wh.y);
}

void Workspace::createClientForWindow(Window w) {
    // emplacing does not invalidate iterators
    m_clients.emplace_front(*this, w);
    m_clients.front().getXWindow().mapRaised();
}

void Workspace::removeClientForWindow(Window w) {
    m_clients.erase(
        std::remove_if(m_clients.begin(), m_clients.end(), [&](Client& c) {
            return c.getXWindow().get() == w;
        }));  // possibly revalidate iterator
    focusFront();
}

void Workspace::removeClient(Client& w) { 
    removeClientForWindow(w.getXWindow().get()); 
}

void Workspace::setSelectedClient(Client& c){
    for (auto i = m_clients.begin(); i != m_clients.end(); i++) {
        if (&c == &(*i))
            selectedClientIter_ = i;
    }
}

void Workspace::moveFocusedClient( int i ) {
    auto& clientPosition = selectedClientIter_;
    // due to how splice works in case of i > 0 correction is needed so splice
    // doesn't leave the list unchanged
    auto nextPosition = circulateWithEndIterator(m_clients, selectedClientIter_,
                                                 (i > 0) ? i + 1 : i);
    //( pos, other, it)
    m_clients.splice(nextPosition, m_clients, clientPosition);
}

void Workspace::moveFocusedClientToTop() {
    m_clients.splice(m_clients.begin(), m_clients, selectedClientIter_);
}

void Workspace::moveSelectedClientToWorkspace( Workspace& other ){
    if (this->selectedClientIter_ == this->m_clients.end()) {
        // std::cout << "MOVE NOTHING" << std::endl;
        return;
    }
    // move client to beggining of the other list
	other.m_clients.splice( other.m_clients.begin(), this->m_clients, this->selectedClientIter_ );
	// assing moved clients reference to workspace to other workspace
	other.m_clients.begin()->assignToWorkspace( other );
	// reset iterators to focused clients
	other.selectedClientIter_ = other.m_clients.begin();
	this->selectedClientIter_ = this->m_clients.begin();
}

