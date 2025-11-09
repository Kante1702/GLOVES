#define _CRT_SECURE_NO_WARNINGS 
#include "GloveValues.hpp"
#include "ImuConfiguration.hpp"
#include <iomanip> //formatovany cas
#include <ctime>
#include <chrono>
#include <sstream>

GloveValues::GloveValues() : GloveConnection() {
	/////////nic
	LoggerBase::setName("GloveValues");

	std::time_t t = std::time(nullptr); //v sekundach surovy cas
	std::tm* now = std::localtime(&t); //rozdeli to na dni, hodiny ...


}

GloveValues::~GloveValues() {
	for (auto& [name, file] : m_logFiles) {
		if (file.is_open()) {
			file.close();
		}
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
	
	//ak sa najde tak left ked nie tak right (npos v podstate znamena hladanie nebolo upesne)
	GSdk::BoardTools::WearingPosition position = (gloveName.find("Left") != std::string::npos || gloveName.find("4305") != std::string::npos)
		? GSdk::BoardTools::WearingPosition::GSdkWearingPositionLeftGlove
		: GSdk::BoardTools::WearingPosition::GSdkWearingPositionRightGlove;

	std::time_t t = std::time(nullptr);
	std::tm* now = std::localtime(&t);
	char filename[128];
	
	std::strftime(filename, sizeof(filename), (position == GSdk::BoardTools::WearingPosition::GSdkWearingPositionLeftGlove)
		? "GloveLog_Left_%Y%m%d_%H%M%S.csv" : "GloveLog_Right_%Y%m%d_%H%M%S.csv",now);

	std::ofstream& logFile = m_logFiles[gloveName];
	logFile.open(filename);
	if (!logFile.is_open()) {
		this->printError("Failed to open log file for " + gloveName);
	}
	else {
		m_headerWritten[gloveName] = false;
	}



	try {
		

		GSdk::BoardTools::ExternalSensorAssembly assembly(position);
		auto tags = assembly.tags();

		std::cout << " DETECTED sensors for " << gloveName << "( " << ((position == GSdk::BoardTools::WearingPosition::GSdkWearingPositionLeftGlove) ? "LEFT " : "RIGHT ")
			<< "hand" << std::endl;
		for (int tag : tags) {

			const auto& sensor = GSdk::BoardTools::ExternalSensor::registeredSensor(tag);
			std::cout << "TAG " << tag << "=> " << sensor.name() << " ( inverted = " << std::boolalpha
				<< sensor.valueInverted() << " )" << std::endl;

		}

	}
	catch(...){
		this->printError("Failed to list sensors");
	}


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
void GloveValues::logToCSV(const std::string& gloveName, const std::vector<uint8_t> values, GSdk::BoardTools::WearingPosition position) {

	auto it = m_logFiles.find(gloveName);
	if (it == m_logFiles.end() || !it->second.is_open())
	{
		return;
	}
		std::ofstream& logFile = it->second;
	


	GSdk::BoardTools::ExternalSensorAssembly assembly(position);

	//casova znamka
	auto now = std::chrono::system_clock::now();
	auto ms = std::chrono::duration_cast<std::chrono::milliseconds> (now.time_since_epoch()) % 1000; //epoch 1.januara 1970 ... %1000 milisekundy aktualnej sekundy

	std::time_t t = std::chrono::system_clock::to_time_t(now);
	std::tm tm{};
	localtime_s(&tm, &t);
	
	char TIMEstr[32];
	std::strftime(TIMEstr, sizeof(TIMEstr), "%H: %M: %S", &tm);
	
	std::ostringstream timestamp;
	timestamp << TIMEstr << "." << std::setfill('0') << std::setw(3) << ms.count();


	//veci na hlavicku
	if (!m_headerWritten[gloveName]) {
		logFile << "Timestamp " +gloveName;

		for (int tag : assembly.tags()) {
			if (m_logOnlyBending && (tag % 2 == 0))continue; //len ohyb preto %2

			const auto& sensor = GSdk::BoardTools::ExternalSensor::registeredSensor(tag);
			logFile << ";" << sensor.name();
		}
		logFile << "\n";
		m_headerWritten[gloveName] = true;

	}
	
	

	//logovanie a rozpoznavanie 
	auto normalizedValues = getNormalizedFingerValues(values,assembly);

	logFile << timestamp.str();
	for (float normalized : normalizedValues) {
		logFile << ";" << normalized;
	}

	logFile << "\n";

	logFile.flush(); //zapisuje data v realnom case , nestratim posledne riadky 

	//gesture recognition - invertujeme poradie pre pravu ruku vo funkcii GestureLibrary::recognize()
	if (normalizedValues.size() >= 5) {
		std::array<float, 5> fingerValues = { 0.0f,0.0f ,0.0f ,0.0f ,0.0f };
		std::copy_n(normalizedValues.begin(), 5 , fingerValues.begin());//source_begin , n , destination_begin
		
		std::optional<std::string> recognized;
		if (position == GSdk::BoardTools::WearingPosition::GSdkWearingPositionLeftGlove) {
			recognized = m_leftGestureRecognizer.recognize(fingerValues, position);
		}
		else {
			recognized = m_rightGestureRecognizer.recognize(fingerValues, position);
		}

		if (recognized) {
			this->printInfo("[" + gloveName + " ] Gesture detected: " + *recognized);
		}

	}


}

std::array<float,5> GloveValues::getNormalizedFingerValues(const std::vector<uint8_t>& values,GSdk::BoardTools::ExternalSensorAssembly assembly) {

	std::array<float, 5> fingerValues = { 0.0f,0.0f ,0.0f ,0.0f ,0.0f };
	size_t count = 0;

	for (int tag : assembly.tags()) {
		if (m_logOnlyBending && (tag % 2 == 0)) {
			continue;
		}
		int index = assembly.findIndex(tag) * 4;
		float val = 0.0f;

		if (index >= 0 && index + 3 < values.size()) {
			std::memcpy(&val, &values[index], sizeof(float));
			
		}

		fingerValues[count] = val;
		count++;
		if (count >= 5) break;
			
	}
	return fingerValues;
}





void GloveValues::subscribe(const std::string& gloveName, std::shared_ptr<GSdk::Board::BoardPeripheral> board) {
	
	

	//viac pozri StreamTimeslots.h
	auto streamTimeslots = GSdk::Board::getEmptyStreamTimeslots(); //potrebujem to vynulovat na zaciatku
	streamTimeslots.sensorsState = 6; //6 alebo GSdkBoardStreamTypeSensorsState; //data o senzore prstov( data represents conductivity)

	/*
	streamTimeslots.taredQuaternion = 6 //ak je treba aj polohu ruky(IMU)
	*/

	if (!board->streamTimeslots().write(streamTimeslots)) {
		this->printError("[ "+ gloveName + " ]"+ "Unable to set stream timeslots");
		return;
	}
	
	GSdk::BoardTools::WearingPosition position =
		(gloveName.find("Left") != std::string::npos || gloveName.find("4305") != std::string::npos)
		? GSdk::BoardTools::WearingPosition::GSdkWearingPositionLeftGlove
		: GSdk::BoardTools::WearingPosition::GSdkWearingPositionRightGlove;

	int streamID = board->streamReceived().connect([this,gloveName,position](const GSdk::Board::BoardStreamEventArgs& args) {
		//GSdkBoardStreamTypeTaredAltitude -> brief Relative altitude from taring point, meters
		if (args.streamType == GSdkBoardStreamTypeSensorsState){
			auto value = args.bytes();




			//cas
			auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
			std::tm tm = *std::localtime(&now);
			char timeStr[32];

			std::strftime(timeStr, sizeof(timeStr), "%H: %M: %S", &tm);


			//this->printInfo("[ "+ gloveName + " ]" + "Received sensorsState data (" + std::to_string(value.size()) + " bytes)");
			
			logToCSV(gloveName, value,position);

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
			if (it != m_streamIDs.end()) {
				board->streamReceived().disconnect(it->second);
			}
		}
	}
	m_streamIDs.clear();
	m_peripherals.clear();

}


