#include "Workspace.h"

void Workspace::arrange( int barHeight, int screenW, int screenH ) {
	if( fullscreenClient_ ){
		fullscreenClient_->resize( 
			point {0,barHeight},
			point {screenW, screenH - barHeight}
		);
		return;
	}
	auto tiledClientsNum = std::count_if( clients_.begin(), clients_.end(), [](const Client& c){ return not c.isFloating(); } );

	if( tiledClientsNum == 1 ){
		clients_.front().resize( 
			point{ config.outerGap,config.outerGap + barHeight }, 
			point{ screenW , screenH - barHeight } - config.outerGap*2
		);
	}
	else{
		int slavesNum   = tiledClientsNum - config.mastersNum;
		int masterW     = screenW * config.factor * 0.01;
		int masterH     = ((screenH-barHeight) - config.outerGap) / config.mastersNum - config.innerGap;
		int masterCurrentY = barHeight + config.innerGap;

		int slaveX      = config.outerGap + masterW + config.innerGap;
		int slaveW      = screenW - slaveX - config.outerGap;
		int slaveH      = ((screenH-barHeight) - config.outerGap) / slavesNum - config.innerGap;
		int currentY    = barHeight + config.outerGap;

		int currentYinc  = slaveH    + config.innerGap;
		int masterYInc   = masterH   + config.innerGap;

		int arrangedMasters = 0;
		for( auto& c : clients_ ) {
			if( c.isFloating() ){
				c.raise();
			}
			else if( arrangedMasters < config.mastersNum ){
				c.resize( 
					point{ config.outerGap, masterCurrentY },
					point{ masterW, masterH } 
				);
				masterCurrentY += masterYInc;
				arrangedMasters++;
			}
			else{
				c.resize(  
					point{ slaveX, currentY }, 
					point{ slaveW, slaveH } 
				);
				currentY += currentYinc;
			}
		}
	}
}

using ClientListIterator = typename std::list<Client>::iterator;
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

Workspace::Workspace( Monitor& m, uint index ) : 
	config_( config ), // defualt config
	monitorRef_( &m ),
	index_( index ),
	clients_(),
	selectedClientIter_( clients_.begin() ), //TODO: is this safe?
	fullscreenClient_( nullptr )
{ }

void Workspace::focusFront() {
	selectedClientIter_ = clients_.begin();
}

void Workspace::toggleFullscreenOnSelectedClient() { 
	fullscreenClient_ = (fullscreenClient_) ? nullptr : & *selectedClientIter_; 
}

void Workspace::moveFocus( int i ){
	selectedClientIter_ = circulate( clients_, selectedClientIter_, i);
}

void Workspace::showAllClients(){
	for( auto& client : clients_ )
		client.show();
}

void Workspace::hideAllClients(){
	for( auto& client : clients_ )
		client.hide();
}
void Workspace::arrangeClients( int barHeight, point wh ){
	arrange(barHeight, wh.x, wh.y );
}

void Workspace::addClient( Window w ){
	XWindowAttributes wa;
	XlibWrapper::instance().getWindowAttributes( w, wa );
	clients_.emplace_front( *this, w, wa );
}

void Workspace::removeClient( Window w ){
	clients_.erase( std::remove_if( clients_.begin(), clients_.end(), 
		[&](Client& c) { 
			return c.getXWindow() == w;
		})
	);// possibly revalidate iterator
	selectedClientIter_ = clients_.begin();
}

void Workspace::removeClient( Client& w ){
	removeClient( w.getXWindow() );	
}

void Workspace::setSelectedClient(Client& c){
	for(auto i = clients_.begin(); i != clients_.end(); i++){
		if( &c == &(*i) )
			selectedClientIter_ = i;
	}
}

void Workspace::moveSelectedClient( int i ) {
	auto& clientPosition = selectedClientIter_;
	// due to how splice works in case of i > 0 correction is needed so splice doesn't leave the list unchanged
	auto nextPosition   = circulateWithEndIterator( clients_, selectedClientIter_, ( i > 0 ) ? i + 1 : i  );
	//( pos, other, it)
	clients_.splice( nextPosition, clients_,  clientPosition);
}

void Workspace::moveSelectedClientToTop() {
	clients_.splice( clients_.begin(), clients_,  selectedClientIter_);
}

void Workspace::moveSelectedClientToWorkspace( Workspace& other ){
	if( this->selectedClientIter_ == this->clients_.end() ){
		std::cout << "MOVE NOTHING" << std::endl;
		return;
	}
	// move client to beggining of the other list
	other.clients_.splice( other.clients_.begin(), this->clients_, this->selectedClientIter_ );
	// assing moved clients reference to workspace to other workspace
	other.clients_.begin()->assignToWorkspace( other );
	// reset iterators to focused clients
	other.selectedClientIter_ = other.clients_.begin();
	this->selectedClientIter_ = this->clients_.begin();
}

