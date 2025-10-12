#include "GloveConnection.hpp"


using namespace GSdk;

GloveConnection::GloveConnection():LoggerBase("GloveConnection") {

	m_central = Board::makeBoardAdapterCentral();


}


GloveConnection::~GloveConnection() {

	disconnect();

}


bool GloveConnection::connect() {


	if (!m_central) {
		return false;
	}

	//zaciname skenovat (5s)
	m_central->scanChanged().connect([this](const Board::CentralScanEventArgs &args) {
		this->printInfo("Central status (changed): " + to_string(args.scan()));
			});

	m_central->peripheralsChanged().connect([this](const Board::CentralPeripheralsEventArgs& args) {
		auto peripherals = args.inserted();
		for (auto peripheral : peripherals) {
			auto board = std::dynamic_pointer_cast<Board::BoardPeripheral> (peripheral);
			if (board) {

				this->printInfo("============ TRYING TO CONNECT PERIPHERAL ============");
				this->printInfo("Found device(ID): " + board->id());
				this->printInfo("Name: "+ board->name());
				m_peripheral = board;


				//sledovanie zmeny statusu rukavice
				m_peripheral->propertyChanged().connect([this](const Board::PeripheralPropertyEventArgs& args) {
					if (args.name == Board::PeripheralProperty::status) {
						this->printInfo("Status changed: " + to_string(this->m_peripheral->status()));
					}
					});

				if (board->start()) {
					
					this->printInfo("Status(after start): "+ to_string(peripheral->status()));
					m_peripheral->emulationState().write(true);

					//zavola sa hook = nadchadzajuci kod moze reagovat
					onPeripheralConnected(board);
				}
				else {
					this->printError("Failed to connect ... Waiting to reconnect... ");
				}

			}
		}
	});

	m_central->startScan(GSdkScanOptionsMake(5));
	return true;

}


void GloveConnection::disconnect() {
	
	if (m_central) {
		m_central->stopScan();
	}

	if (m_central) {
		m_peripheral->stop();

		//zavolanie hooku pri odpojeni
		onPeripheralDisconnected();

		m_peripheral.reset();
	}

}






