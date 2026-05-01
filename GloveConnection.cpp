// GloveConnection.cpp
// Implementacia BT spravy pripojenia cez CaptoGlove SDK.

#include "GloveConnection.hpp"
#include <conio.h>
#include <thread>

using namespace GSdk;

GloveConnection::GloveConnection():LoggerBase("GloveConnection") {
	// Vytvorenie BT central managera - vstupny bod SDK
	m_central = Board::makeBoardAdapterCentral();
}


GloveConnection::~GloveConnection() {

	disconnect();

}


bool GloveConnection::connect() {


	if (!m_central) {
		return false;
	}

	// Callback pri zmene stavu skenovania - informatívny vypis
	m_central->scanChanged().connect([this](const Board::CentralScanEventArgs &args) {
		this->printInfo("Central status (changed): " + to_string(args.scan()));
			});

	// Callback pri detekcii noveho BT zariadenia v okolí
	m_central->peripheralsChanged().connect([this](const Board::CentralPeripheralsEventArgs& args) {
		auto peripherals = args.inserted();
		for (auto peripheral : peripherals) {
			auto board = std::dynamic_pointer_cast<Board::BoardPeripheral> (peripheral);
			if (board) {

				this->printInfo("\n============ TRYING TO CONNECT PERIPHERAL ============");
				this->printInfo("Found device(ID): " + board->id());
				this->printInfo("Name: "+ board->name());
				m_peripheral = board;


				// Sledovanie zmeny stavu pripojenia rukavice pocas celej doby behu
				m_peripheral->propertyChanged().connect([this](const Board::PeripheralPropertyEventArgs& args) {
					if (args.name == Board::PeripheralProperty::status) {
						this->printInfo("Status changed: " + to_string(this->m_peripheral->status()));
					}
					});

				if (board->start()) {
					
					this->printInfo("Status(after start): "+ to_string(peripheral->status()));
					disableHID(board);					// vypnutie HID pred zacatim prijímania dat
					onPeripheralConnected(board);		// notifikacia odvodenej triedy
				}
				else {
					this->printError("Failed to connect ... Waiting to reconnect... ");
				}

			}
		}
	});

	// Spustenie BT skenovania s casovym limitom 5 sekund
	m_central->startScan(GSdkScanOptionsMake(5));
	return true;

}


void GloveConnection::disconnect() {
	
	if (m_central) {
		m_central->stopScan();
	}

	if (m_peripheral) {
		onPeripheralDisconnected(); // najprv necháme odvodenú triedu vyèisti streamy
		
		// Kratka pauza pre dokoncenie vsetkych callbackov pred zastavenim periferneho zariadenia
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

		m_peripheral->stop();
		m_peripheral.reset();
	}

}


void GloveConnection::disableHID(std::shared_ptr<GSdk::Board::BoardPeripheral> board) {

	GSdk::Board::EmulationModes modes = GSdk::Board::getEmptyEmulationModes();

	// Vypnutie vsetkych HID rezimov - pouzivame len surove senzorove data prstov
	modes.fingers = false;
	modes.acceleration = false;
	modes.barometer = false;
	modes.inputAxis = false;
	modes.joystick = false;
	modes.mouse = false;

	if (!board->emulationModes().write(modes)) {
		this->printError("Failed to write emulation modes");
		return;
	}

	

	if (!board->emulationState().write(false)) {
		this->printError("Failed to enable emulation state");
		return;
	}
}



