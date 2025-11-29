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
	
	GSdk::BoardTools::WearingPosition position;

	////////////////////////////////
	//urcenie podla id
	//////////////
	if (gloveName.find("4305") != std::string::npos) {
		m_leftGloveName = gloveName;
		position = GSdk::BoardTools::WearingPosition::GSdkWearingPositionLeftGlove;
	}
	// RIGHT = 4272
	else if (gloveName.find("4272") != std::string::npos) {
		m_rightGloveName = gloveName;
		position = GSdk::BoardTools::WearingPosition::GSdkWearingPositionRightGlove;
	}
	else {
		this->printError("Unknown glove detected: " + gloveName);
		return; 
	}



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


void GloveValues::startCalibrating() {
	m_calibrationManager.startCalibration();
	m_calibrationState = CalibrationState::CalibratingLeftOpen;
	this->printInfo("[Calibration] For 3-5s move freely to max and min then put LEFT hand fully OPEN and press 's' or 'S'");

}

void GloveValues::confirmCalibrationStep(const std::array<float, 5>& rawleft, const std::array<float, 5>& rawright) {

	switch (m_calibrationState)
	{
	
	case CalibrationState::CalibratingLeftOpen:

		m_calibrationManager.setOpenHand("Left", rawleft);
		this->printInfo("[Calibration] LEFT OPEN stored. Make LEFT FIST!");
		m_calibrationState = CalibrationState::CalibratingLeftFist;
		break;

	case CalibrationState::CalibratingLeftFist:
		m_calibrationManager.setFist("Left", rawleft);
		this->printInfo("[Calibration] LEFT FIST stored. Put RIGHT hand fully OPEN.");
		m_calibrationState = CalibrationState::CalibratingRightOpen;
		break;

	case CalibrationState::CalibratingRightOpen:

		m_calibrationManager.setOpenHand("Right", rawright);
		this->printInfo("[Calibration] RIGHT OPEN stored. Make RIGHT FIST!");
		m_calibrationState = CalibrationState::CalibratingRightFist;
		break;

	case CalibrationState::CalibratingRightFist:

		m_calibrationManager.setFist("Right", rawright);
		this->printInfo("[Calibration] RIGHT FIST stored. Calibration DONE!");

		//dokoncenie
		m_calibrationManager.finishCalibration();
		m_calibrationManager.saveToCSV("Left");
		m_calibrationManager.saveToCSV("Right");
		m_calibrationState = CalibrationState::Done;
		break;
	
	case CalibrationState::Done:
		printInfo("[Calibration] Saved/finished calibration files.");
		break;
	default:
		break;
	}


}

void GloveValues::updateGestures() {

}

void GloveValues::handleLeftGesture(const std::string& gestureName, const std::array<float, 5>& values) {
	// zatial iba vypis
	printInfo("[LEFT GESTURE] Detected: " + gestureName);
}

void GloveValues::handleRightControl(const std::string& gestureName, const std::array<float, 5>& values) {
	// zatial iba vypis
	printInfo("[RIGHT GESTURE] Detected: " + gestureName);
}


//kriticka cast ktora by mala byt co najrychlejsia preto v mili sekundach
void GloveValues::logToCSV(const std::string& gloveName, const std::vector<uint8_t> values, GSdk::BoardTools::WearingPosition position) {

	auto it = m_logFiles.find(gloveName);
	if (it == m_logFiles.end() || !it->second.is_open())
	{
		return;
	}
		std::ofstream& logFile = it->second;
	
	///klucove urcenie strany
		std::string sideKEY = (position == GSdk::BoardTools::WearingPosition::GSdkWearingPositionLeftGlove) ?
			"Left" : "Right";




	GSdk::BoardTools::ExternalSensorAssembly assembly(position);
	// ------------------------------------------------------------
	//casova znamka
	// ------------------------------------------------------------
	auto now = std::chrono::system_clock::now();
	auto ms = std::chrono::duration_cast<std::chrono::milliseconds> (now.time_since_epoch()) % 1000; //epoch 1.januara 1970 ... %1000 milisekundy aktualnej sekundy

	std::time_t t = std::chrono::system_clock::to_time_t(now);
	std::tm tm{};
	localtime_s(&tm, &t);
	
	char TIMEstr[32];
	std::strftime(TIMEstr, sizeof(TIMEstr), "%H: %M: %S", &tm);
	
	std::ostringstream timestamp;
	timestamp << TIMEstr << "." << std::setfill('0') << std::setw(3) << ms.count();

	// ------------------------------------------------------------
	//veci na hlavicku
	// ------------------------------------------------------------
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
	
	
	// ------------------------------------------------------------
	// raw normalizovane 0-1
	// ------------------------------------------------------------
	auto rawValues = getNormalizedFingerValues(values,assembly);
	
	std::array<float, 5> fingerRaw{};
	std::copy_n(rawValues.begin(), 5, fingerRaw.begin());

	// ------------------------------------------------------------
	// kalibracia - ak prebieha zbierame len raw hodnoty
	// ------------------------------------------------------------


	if (m_calibrationManager.m_isCalibrating) {
		m_calibrationManager.updateRaw(sideKEY, fingerRaw);


		if (position == GSdk::BoardTools::WearingPosition::GSdkWearingPositionLeftGlove)
			m_lastRawLeft = fingerRaw;
		else
			m_lastRawRight = fingerRaw;



		logFile << timestamp.str();
		for (float f : fingerRaw) {
			logFile << ";" << f;
		}
		logFile << "\n";
		logFile.flush();
		return;

	}



	// ------------------------------------------------------------
	//normalizacia podla kalibracie 
	// ------------------------------------------------------------

	auto normalizedValues = m_calibrationManager.normalize(sideKEY,  fingerRaw);

	// ------------------------------------------------------------
	//logovanie normalizovanych hodnot 
	// ------------------------------------------------------------
	logFile << timestamp.str();
	for (float normalized : normalizedValues) {
		logFile << ";" << normalized;
	}

	logFile << "\n";

	logFile.flush(); //zapisuje data v realnom case , nestratim posledne riadky 

	// ------------------------------------------------------------
	// mapa prstov
	// ------------------------------------------------------------

	//korektne mapovanie prstov pre lavu ruku
	std::array<float, 5> fingerValues;
		
		
		if (position == GSdk::BoardTools::WearingPosition::GSdkWearingPositionLeftGlove) {
			fingerValues[0] = normalizedValues[4]; //thumb 
			fingerValues[1] = normalizedValues[3]; //index
			fingerValues[2] = normalizedValues[2]; //middle
			fingerValues[3] = normalizedValues[1]; //ring
			fingerValues[4] = normalizedValues[0]; //pinky
		}
		else {
			fingerValues = normalizedValues;
		}

		if (!m_calibrationManager.isCalibrated(sideKEY)) {
			return;
		}


		//rozpoznavanie
		auto recognized = (position == GSdk::BoardTools::WearingPosition::GSdkWearingPositionLeftGlove) 
			? m_leftGestureRecognizer.recognize(fingerValues, position) 
			: m_rightGestureRecognizer.recognize(fingerValues, position);
		
		
		if (recognized) {
			this->printInfo("[" + gloveName + " ] Gesture detected: " + *recognized);
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
		( gloveName.find("4305") != std::string::npos)
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


