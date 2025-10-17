#define _CRT_SECURE_NO_WARNINGS 
#include "GloveValues.hpp"
#include "ImuConfiguration.hpp"
#include <iomanip> //formatovany cas
#include <ctime>
#include <chrono>

GloveValues::GloveValues() : GloveConnection() {
	/////////nic
	LoggerBase::setName("GloveValues");

	std::time_t t = std::time(nullptr); //v sekundach surovy cas
	std::tm* now = std::localtime(&t); //rozdeli to na dni, hodiny ...

	char filename[64];
	std::strftime(filename, sizeof(filename), "GloveLOG_%Y%m%d_%H%M%S.csv", now);
	m_logFile.open(filename);

	if (m_logFile.is_open()) {
		m_logFile << "Timestamp, GloveName, FingerIndex, Value\n";
	}
	else {
		this->printError("Failed to open log fike for writing");
	}


}

GloveValues::~GloveValues() {
	if (m_logFile.is_open()) {
		m_logFile.close();
	}
	//pre ochranu aj tu je unsubscribe
	if (!m_streamIDs.empty()) {
		unsubscribe();
	}


}

void GloveValues::onPeripheralConnected(std::shared_ptr<GSdk::Board::BoardPeripheral> board) {
	
	std::string gloveName = board->name();
	m_peripherals[gloveName] = board;

	this->printInfo("GloveValues: Peripheral connected ... Starting subscribe ...");
	subscribe(gloveName , board);

	//ak je trieda ImuConfiguration tak volame citanie 
	auto imuConfigPtr = dynamic_cast<ImuConfiguration*>(this);
	if (imuConfigPtr) {
		imuConfigPtr->readImu();
	}

}

void GloveValues::onPeripheralDisconnected() {
	this->printInfo("GloveValues: Peripheral disconnected ... unsubscribing ...");
	unsubscribe();
}

//kriticka cast ktora by mala byt co najrychlejsia preto v mili sekundach
void GloveValues::logToCSV(const std::string& gloveName, const std::vector<uint8_t> values) {

	if (!m_logFile.is_open()) {
		return;
	}

	auto now = std::chrono::system_clock::now();
	auto ms = std::chrono::duration_cast<std::chrono::milliseconds> (now.time_since_epoch()) % 1000; //epoch 1.januara 1970 ... %1000 milisekundy aktualnej sekundy

	std::time_t t = std::chrono::system_clock::to_time_t(now);
	std::tm tm = *std::localtime(&t);
	char TIMEstr[32];
	std::strftime(TIMEstr, sizeof(TIMEstr), "%H: %M: %S", &tm);
	
	std::ostringstream timestamp;
	timestamp << TIMEstr << "." << std::setfill('0') << std::setw(3) << ms.count();

	//NORMOVANIE HODNOT SENZORA OD 0.0 - 1.0
	std::vector<float> normalizedValues;
	normalizedValues.reserve(values.size());

	for (auto val : values) {
		normalizedValues.push_back(static_cast<float>(val) / 255.0f);
	}

	m_logFile << timestamp.str() << gloveName;

	for (auto nv : normalizedValues) {
		m_logFile << "," << nv;
	}
	m_logFile << "\n";

	m_logFile.flush(); //zapisuje data v realnom case , nestratim posledne riadky 
}







void GloveValues::subscribe(const std::string& gloveName, std::shared_ptr<GSdk::Board::BoardPeripheral> board) {
	

	//viac pozri StreamTimeslots.h
	auto streamTimeslots = GSdk::Board::getEmptyStreamTimeslots(); //potrebujem to vynulovat na zaciatku
	streamTimeslots.sensorsState = 6; //data o senzore prstov( data represents conductivity)

	/*
	streamTimeslots.taredQuaternion = 6 //ak je treba aj polohu ruky
	*/

	if (!board->streamTimeslots().write(streamTimeslots)) {
		this->printError("[ "+ gloveName + " ]"+ "Unable to set stream timeslots");
		return;
	}


	int streamID = board->streamReceived().connect([this,gloveName](const GSdk::Board::BoardStreamEventArgs& args) {
		//GSdkBoardStreamTypeTaredAltitude -> brief Relative altitude from taring point, meters
		if (args.streamType == GSdkBoardStreamTypeSensorsState){
			auto value = args.bytes();




			//cas
			auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
			std::tm tm = *std::localtime(&now);
			char timeStr[32];

			std::strftime(timeStr, sizeof(timeStr), "%H: %M: %S", &tm);


			//this->printInfo("[ "+ gloveName + " ]" + "Received sensorsState data (" + std::to_string(value.size()) + " bytes)");
			
			logToCSV(gloveName, value);
			

		}


		//GSdkBoardStreamTypeTaredQuaternion -> brief Tared relative reference orientation quaternion
		if (args.streamType == GSdkBoardStreamTypeTaredQuaternion) {
			auto quatArgs = static_cast<const GSdk::Board::BoardQuaternionfEventArgs&> (args);
			auto quatV = quatArgs.value();
			this->printInfo("Received tared quaternion " + getString(quatV));
		}
	});

	m_streamIDs[gloveName] = streamID;

}


void GloveValues::unsubscribe() {

	for(auto & [gloveName, board] : m_peripherals) {
		if (board) {
			auto it = m_streamIDs.find(gloveName);
			if(it != m_streamIDs.end())
			board->streamReceived().disconnect(it->second);

		}
	}
	m_streamIDs.clear();
	m_peripherals.clear();

}



/*


				dokoncit treba rozoznat ktora rukavica co posiela napr pomocou kontajnera ako je 
				vector alebo map


*/

