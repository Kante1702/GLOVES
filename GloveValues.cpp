// GloveValues.cpp
// Implementacia jadra systemu - spracovanie dat rukavic, kalibracia, gesta, TCP.

#define _CRT_SECURE_NO_WARNINGS
#include "GloveValues.hpp"
#include "ImuConfiguration.hpp"
#include <iomanip> //formatovany cas
#include <ctime>
#include <chrono>
#include <sstream>

GloveValues::GloveValues() : GloveConnection() {
	
	LoggerBase::setName("GloveValues");

	std::time_t t = std::time(nullptr);
	std::tm* now = std::localtime(&t); 


	// Nacitanie konfiguracie (port robota, ID rukavic) zo suboru config.txt
	loadConfig("config.txt");

	// Spustenie TCP servera a blokujuce cakanie na pripojenie robota
	m_robotServer = std::make_unique<RobotCommunicationServer>(m_robotPort);
	if (m_robotServer->startServer()) {
		this->printInfo("[TCP] Server started on port "+ std::to_string(m_robotPort));
	}
	
	if (m_robotServer->waitForRobot()) {
		this->printInfo("[TCP] Robot connected");
	}


	//// ====================== Treba odkomentovat aby boli rozoznane v experimente (pozri aj GestureLibrary.hpp =========================
	m_experiment.setGestures({
	// Testovacie gesta pridane pre experiment
	
	//{"RIGHT","Gesto_1"},
	//{"RIGHT","Gesto_2"},
	//{"RIGHT","Gesto_3"},

	//{"LEFT","Gesto_4"},
	//{"LEFT","Gesto_5"},
	//{"LEFT","Gesto_6"},
	

	// Systemove gesta pravej ruky
	{"RIGHT","Lock_Unlock"},
	{"RIGHT","Right_X"},
	{"RIGHT","Right_Y"},
	{"RIGHT","Right_Z"},
	{"RIGHT","Right_Yaw"},
	{"RIGHT","Right_Pitch"},
	{"RIGHT","Right_Roll"},
	{"RIGHT","Right_Custom_Mode"},
	{"RIGHT","Right_RobotDisconnect"},
	{"RIGHT","Right_Stop_Resume"},
		
	// Systemove gesta lavej ruky 
	{"LEFT","Left_XYZ_Mode"},
	{"LEFT","Left_Rotation_Mode"},
	{"LEFT","Left_Custom_Mode"},
	//{"LEFT","Left_Service_Mode" },
	{"LEFT","Left_Positive_Direction"},
	{"LEFT","Left_Negative_Direction"},
	{"LEFT","Left_Stop_Resume" },
	
	});
	

}

GloveValues::~GloveValues() {
	for (auto& [name, file] : m_logFiles) {
		if (file.is_open()) {
			file.close();
		}
	}

	for (auto& [name, file] : m_rawLogFiles) {
		if (file.is_open()) {
			file.close();
		}
	}
	if (m_robotServer) {
		m_robotServer->stopServer();
	}
}

void GloveValues::loadConfig(const std::string& filename) {

	std::ifstream file(filename);
	std::string line;

	
	// Predvolene hodnoty pouzite ak subor config.txt neexistuje
	m_robotPort = 23432;
	m_leftGloveIdStr = "4305";
	m_rightGloveIdStr = "4272";
	

	if (file.is_open()) {
		while (std::getline(file, line)) {
			std::istringstream is_line(line);
			std::string key;
			if (std::getline(is_line, key, '=')) {
				std::string value;
				if (std::getline(is_line, value)) {
					if (key == "Robot_Port") {
						m_robotPort = std::stoi(value);
					}
					else if (key == "LeftGlove_ID") {
						m_leftGloveIdStr = value;
					}
					else if (key == "RightGlove_ID") {
						m_rightGloveIdStr = value;
					}
				}
			}

		}
		file.close();
		this->printInfo("Config loaded from " + filename);
	}
	else {
		this->printInfo("Config file not found, using defaults");
	}
}

//// =========================== Treba odkomentovat ak chceme posielat dalsie commandy (ak ano treba dorobit nove podmienky v TP programe) ======================

std::string GloveValues::gesturesToCommand(const std::string& gesture) {

	// HMG kodovanie: H = ruka (1=prava, 2=lava), M = rezim (1=XYZ, 2=Rot, 3=Custom, 0=system), G = prikaz
	if (gesture == "Right_X") return "110";
	if (gesture == "Right_Y") return "111";
	if (gesture == "Right_Z") return "112";
	if (gesture == "Right_Yaw") return "120";
	if (gesture == "Right_Pitch") return "121";
	if (gesture == "Right_Roll") return "122";
	if (gesture == "Right_RobotDisconnect") return "130";
	//if (gesture == "Right_Custom_Mode") return "131";
	if (gesture == "Right_Stop_Resume") return "101";
	//if (gesture == "Left_XYZ_Mode") return "240";
	//if (gesture == "Left_Rotation_Mode") return "241";
	//if (gesture == "Left_Custom_Mode") return "242";
	//if (gesture == "Left_Service_Mode") return "243";
	if (gesture == "Left_Positive_Direction") return "250";
	if (gesture == "Left_Negative_Direction") return "251";
	if (gesture == "Left_Stop_Resume") return "201";
	return "";  // gesto bez priradeného prikazu (napr. prepinanie rezimov - riadene lokalne)
}


bool GloveValues::sendManualCommand(const std::string& rawCommand) {
	if (!m_robotServer) {
		return false;
	}
	return m_robotServer->sendCommand(rawCommand);

}




void GloveValues::onPeripheralConnected(std::shared_ptr<GSdk::Board::BoardPeripheral> board) {

	std::string gloveName = board->name();
	m_peripherals[gloveName] = board;

	this->printInfo("GloveValues: Peripheral connected ... Starting subscribe ...");


	// Identifikacia strany rukavice podla ID obsiahnuteho v nazve BT zariadenia
	GSdk::BoardTools::WearingPosition position;
	if (gloveName.find(m_leftGloveIdStr) != std::string::npos) {
		m_leftGloveName = gloveName;
		position = GSdk::BoardTools::WearingPosition::GSdkWearingPositionLeftGlove;
	}
	// RIGHT = 4272
	else if (gloveName.find(m_rightGloveIdStr) != std::string::npos) {
		m_rightGloveName = gloveName;
		position = GSdk::BoardTools::WearingPosition::GSdkWearingPositionRightGlove;
	}
	else {
		this->printError("Unknown glove detected: " + gloveName);
		return;
	}


	// Otvorenie CSV suborov s casovou znackou v nazve pre jednoznacnu identifikaciu merania
	std::time_t t = std::time(nullptr);
	std::tm* now = std::localtime(&t);
	char filename[128];

	std::strftime(filename, sizeof(filename), (position == GSdk::BoardTools::WearingPosition::GSdkWearingPositionLeftGlove)
		? "GloveLog_Left_%Y%m%d_%H%M%S.csv" : "GloveLog_Right_%Y%m%d_%H%M%S.csv", now);

	std::ofstream& logFile = m_logFiles[gloveName];
	logFile.open(filename);
	if (!logFile.is_open()) {
		this->printError("Failed to open log file for " + gloveName);
	}
	else {
		m_headerWritten[gloveName] = false;
	}

	//raw
	char rawFilename[128];

	std::strftime(
		rawFilename,
		sizeof(rawFilename),
		(position == GSdk::BoardTools::WearingPosition::GSdkWearingPositionLeftGlove)
		? "GloveRAW_Left_%Y%m%d_%H%M%S.csv"
		: "GloveRAW_Right_%Y%m%d_%H%M%S.csv",
		now
	);

	std::ofstream& rawFile = m_rawLogFiles[gloveName];
	rawFile.open(rawFilename);
	m_rawHeaderWritten[gloveName] = false;

	// Vypis zoznamu detekovanych senzorov pre diagnosticke ucely
	try {


		GSdk::BoardTools::ExternalSensorAssembly assembly(position);

		std::cout << "ASSEMBLY TAGS COUNT for " << gloveName
			<< " = " << assembly.tags().size() << std::endl;
		auto tags = assembly.tags();

		std::cout << " DETECTED sensors for " << gloveName << "( " << ((position == GSdk::BoardTools::WearingPosition::GSdkWearingPositionLeftGlove) ? "LEFT " : "RIGHT ")
			<< "hand" << std::endl;
		for (int tag : tags) {

			const auto& sensor = GSdk::BoardTools::ExternalSensor::registeredSensor(tag);
			std::cout << "TAG " << tag << "=> " << sensor.name() << " ( inverted = " << std::boolalpha
				<< sensor.valueInverted() << " )" << std::endl;

		}

	}
	catch (...) {
		this->printError("Failed to list sensors");
	}


	subscribe(gloveName, board);

	// Nacitanie IMU konfigurácie ak je tato trieda instanciou ImuConfiguration
	auto imuConfigPtr = dynamic_cast<ImuConfiguration*>(this);
	if (imuConfigPtr) {
		imuConfigPtr->readImu();
	}

}

void GloveValues::onPeripheralDisconnected() {

	this->printInfo("GloveValues: Peripheral disconnected ... unsubscribing ...");

	for (auto& [name, board] : m_peripherals) {
		if (board) {
			// Zastavenie BT streamu - uvolnenie Bluetooth kanálu
			auto empty = GSdk::Board::getEmptyStreamTimeslots();
			board->streamTimeslots().write(empty);

			// Odpojenie callbacku pomocou ulozeneho ID streamu
			if (m_streamIDs.count(name)) {
				board->streamReceived().disconnect(m_streamIDs[name]);
			}
		}
	}

	m_streamIDs.clear();
	for (auto& [name, file] : m_logFiles) {
		if (file.is_open()) file.close();
	}
	for (auto& [name, file] : m_rawLogFiles) {
		if (file.is_open()) file.close();
	}

}

void GloveValues::disconnect() {
	
	this->onPeripheralDisconnected();

	GloveConnection::disconnect();
}

void GloveValues::startCalibrating() {
	m_calibrationManager.startCalibration();
	m_calibrationState = CalibrationState::CalibratingLeftOpen;
	this->printInfo("[Calibration] put LEFT hand fully OPEN and press 's' or 'S'");

}

void GloveValues::startExperiment() {

	if (m_experiment.isRunning()) {
		this->printInfo("[EXPERIMENT] Already running\n");
		return;
	}


	if (!m_calibrationManager.isCalibrated("Left") || !m_calibrationManager.isCalibrated("Right")) {
		this->printInfo("[Experiment] Calibration not done ... Please calibrate first");
		return;
	}
	m_experimentMode = true;
	this->printInfo("[Experiment] Experiment mode ON");
	
	// Skratenie casoveho okna na 1 ms pre okamzitu detekciu pocas experimentu
	m_leftGestureRecognizer.setExperimentMode(true);
	m_rightGestureRecognizer.setExperimentMode(true);
	m_experiment.start();
}

//Funkcia obnovujuca povodne casove okna inak pocas experimentu casove okna = 1ms
void GloveValues::stopExperiment() {
	m_experimentMode = false;
	m_leftGestureRecognizer.setExperimentMode(false);
	m_rightGestureRecognizer.setExperimentMode(false);
	m_experiment.stopEarly();
}


void GloveValues::confirmCalibrationStep(const std::array<float, 5>& rawleft,const std::array<float, 5>& rawright)
{
	if (!m_calibrationManager.m_isCalibrating) return;

	// Spustenie zberu vzoriek pre aktualny krok kalibracie
	switch (m_calibrationState)
	{
	case CalibrationState::CalibratingLeftOpen:
		this->printInfo("[Calibration] Collecting LEFT OPEN samples...");
		m_collecting = true;
		break;

	case CalibrationState::CalibratingLeftFist:
		this->printInfo("[Calibration] Collecting LEFT FIST samples...");
		m_collecting = true;
		break;

	case CalibrationState::CalibratingRightOpen:
		this->printInfo("[Calibration] Collecting RIGHT OPEN samples...");
		m_collecting = true;
		break;

	case CalibrationState::CalibratingRightFist:
		this->printInfo("[Calibration] Collecting RIGHT FIST samples...");
		m_collecting = true;
		break;

	default:
		break;
	}
}

void GloveValues::logRawToCSV(
	const std::string& gloveName,
	const std::vector<uint8_t>& values,
	GSdk::BoardTools::WearingPosition position)
{
	auto it = m_rawLogFiles.find(gloveName);
	if (it == m_rawLogFiles.end() || !it->second.is_open())
		return;

	std::ofstream& rawFile = it->second;

	GSdk::BoardTools::ExternalSensorAssembly assembly(position);

	// Casova znacka s presnostou na milisekundy
	auto now = std::chrono::system_clock::now();
	auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
		now.time_since_epoch()) % 1000;

	std::time_t t = std::chrono::system_clock::to_time_t(now);
	std::tm tm{};
	localtime_s(&tm, &t);

	char timeStr[32];
	std::strftime(timeStr, sizeof(timeStr), "%H:%M:%S", &tm);

	// Hlavicka CSV - zapisuje sa iba raz na zaciatku suboru
	if (!m_rawHeaderWritten[gloveName]) {
		rawFile << "Timestamp_";

		for (int tag : assembly.tags()) {
			if (m_logOnlyBending && (tag % 2 == 0))
				continue;

			const auto& sensor = GSdk::BoardTools::ExternalSensor::registeredSensor(tag);
			rawFile << ";" << sensor.name();
		}

		rawFile << "\n";
		m_rawHeaderWritten[gloveName] = true;
	}

	rawFile << timeStr << "." << std::setw(3) << std::setfill('0') << ms.count();

	for (int tag : assembly.tags()) {
		if (m_logOnlyBending && (tag % 2 == 0))
			continue;

		int index = assembly.findIndex(tag) * 4; // kazda float hodnota zabera 4 bajty
		float val = 0.0f;

		if (index >= 0 && index + 3 < values.size()) {
			std::memcpy(&val, &values[index], sizeof(float));
		}

		rawFile << ";" << val;
	}


	rawFile << "\n";
	rawFile.flush();
}


void GloveValues::handleLeftGesture(const std::string& gesture) {

	if (!m_leftHandEnabled) {
		return;
	}

	// Bezpecnostne gesto - ma prioritu pred vsetkymi ostatnymi
	if (gesture == "Left_Stop_Resume") {
		this->printInfo("[EMERGENCY][LEFT] STOP / RESUME");
		if (m_robotServer) {
			std::string command = gesturesToCommand(gesture);
			if (!command.empty())
			{
				m_robotServer->sendCommand(command);
			}
		}
		return;
	}

	if (gesture == "Gesto_4") {
		this->printInfo("Gesto_4");
	}
	if (gesture == "Gesto_5") {
		this->printInfo("Gesto_5");
	}
	if (gesture == "Gesto_6") {
		this->printInfo("Gesto_6");
	}


	// Prepinanie rezimu riadenia robota
	if (gesture == "Left_XYZ_Mode") {
		m_currentMode = ControlMode::XYZ;
		this->printInfo("-------------- [MODE] XYZ --------------");

	}
	else if (gesture == "Left_Rotation_Mode") {
		m_currentMode = ControlMode::Rotation;
		this->printInfo("-------------- [MODE] ROTATION --------------");
	}

	else if (gesture == "Left_Custom_Mode") {
		m_currentMode = ControlMode::Custom;
		this->printInfo("-------------- [MODE] CUSTOM --------------");
	}

	else if (gesture == "Left_Service_Mode") {
		//m_currentMode = ControlMode::Service; doplnit ked tak
		this->printInfo("-------------- [MODE] SERVICE --------------");
	}

	else if (gesture == "Left_Positive_Direction") {
		this->printInfo("[LEFT] Positive direction");
	}
	else if (gesture == "Left_Negative_Direction") {
		this->printInfo("[LEFT] Negative direction");
	}

	if (m_robotServer) {

		std::string command = gesturesToCommand(gesture);
		if (!command.empty()) {

			m_robotServer->sendCommand(command);
		}
	}

}


void GloveValues::handleRightGesture(const std::string& gesture) {
	// Globalne gesto - zapina/vypina spracovanie gest lavej ruky
	if (gesture == "Lock_Unlock") {
		m_leftHandEnabled = !m_leftHandEnabled;
		this->printInfo("[SYSTEM] Left hand " + std::string(m_leftHandEnabled ? "ENABLED" : "DISABLED"));
		return;
	}

	// Bezpecnostne gesto - bypasuje filter rezimu
	if (gesture == "Right_Stop_Resume") {
		this->printInfo("[EMERGENCY][RIGHT] STOP / RESUME");
		if (m_robotServer) {
			std::string command = gesturesToCommand(gesture);
			if (!command.empty()) {
				m_robotServer->sendCommand(command);
			}
		}

		return;
	}

	// Blokovanie gest, ktore nie su povolene v aktualnom rezime
	if (!isRightGestureAllowed(gesture)) {
		return;
	}
	this->printInfo("[RIGHT][" + std::to_string((int)m_currentMode) + "] " + gesture);

	if (m_robotServer) {

		std::string command = gesturesToCommand(gesture);
		if (!command.empty()) {
			m_robotServer->sendCommand(command );
		}
	}


}


bool GloveValues::isRightGestureAllowed(const std::string& gesture) const {

	// Bezpecnostne gesta su vzdy povolene bez ohladu na rezim
	if (m_globalSafetyGestures.count(gesture)) {
		return true;
	}
	// Systemove gesta su vzdy povolene
	if (gesture == "Lock_Unlock" || gesture == "Gesto_1" || gesture == "Gesto_2" || gesture == "Gesto_3") {
		return true;
	}

	// Kontrola mnoziny povolených gest pre aktualny rezim
	auto it = m_allowedRightGesture.find(m_currentMode);
	if (it == m_allowedRightGesture.end()) {
		return false;
	}

	return it->second.count(gesture) > 0;
}




// Kriticka funkcia volana pri kazdom BT frame - musi byt co najrychlejsia
void GloveValues::logToCSV(const std::string& gloveName, const std::vector<uint8_t> values, GSdk::BoardTools::WearingPosition position) {

	auto it = m_logFiles.find(gloveName);
	if (it == m_logFiles.end() || !it->second.is_open())
		return;

	std::ofstream& logFile = it->second;

	const bool isLeft = (position == GSdk::BoardTools::WearingPosition::GSdkWearingPositionLeftGlove);

	const std::string sideKEY = isLeft ? "Left" : "Right";

	GSdk::BoardTools::ExternalSensorAssembly assembly(position);

	// Casova znacka s presnostou na milisekundy
	auto now = std::chrono::system_clock::now();
	auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
		now.time_since_epoch()) % 1000;

	std::time_t t = std::chrono::system_clock::to_time_t(now);
	std::tm tm{};
	localtime_s(&tm, &t);

	char TIMEstr[32];
	std::strftime(TIMEstr, sizeof(TIMEstr), "%H:%M:%S", &tm);

	std::ostringstream timestamp;
	timestamp << TIMEstr << "." << std::setfill('0') << std::setw(3) << ms.count();

	// Hlavicka CSV - zapisuje sa iba raz na zaciatku suboru
	if (!m_headerWritten[gloveName]) {
		logFile << "Timestamp_" << gloveName;

		for (int tag : assembly.tags()) {
			if (m_logOnlyBending && (tag % 2 == 0))
				continue;

			const auto& sensor =
				GSdk::BoardTools::ExternalSensor::registeredSensor(tag);
			logFile << ";" << sensor.name();
		}

		logFile << "\n";
		m_headerWritten[gloveName] = true;
	}

	// Extrakcia surovych hodnot v HW poradí senzorov
	auto rawValues = getNormalizedFingerValues(values, assembly);
	std::array<float, 5> fingerRaw{};
	std::copy_n(rawValues.begin(), 5, fingerRaw.begin());
	
	// --- KALIBRACIA - zber prvych 100 vzoriek po stlaceni 's' ---
	if (m_calibrationManager.m_isCalibrating && m_collecting)
	{
		switch (m_calibrationState)
		{
		case CalibrationState::CalibratingLeftOpen:
			if (isLeft)
			{
				if (m_calibrationManager.collectOpenSamples(sideKEY, fingerRaw))
				{
					std::cout << "Left open done\n";
					this->printInfo("[Calibration] put LEFT hand fully CLOSED and press 's' or 'S' and WAIT");
					m_collecting = false;
					m_calibrationState = CalibrationState::CalibratingLeftFist;
				}
			}
			break;

		case CalibrationState::CalibratingLeftFist:
			if (isLeft)
			{
				if (m_calibrationManager.collectFistSamples(sideKEY, fingerRaw))
				{
					std::cout << "Left fist done\n";
					this->printInfo("[Calibration] put Right hand fully OPEN and press 's' or 'S' and WAIT");
					m_collecting = false;
					m_calibrationState = CalibrationState::CalibratingRightOpen;
				}
			}
			break;

		case CalibrationState::CalibratingRightOpen:
			if (!isLeft)
			{
				if (m_calibrationManager.collectOpenSamples(sideKEY, fingerRaw))
				{
					std::cout << "Right open done\n";
					this->printInfo("[Calibration] put RIGHT hand fully CLOSED and press 's' or 'S' and WAIT");
					m_collecting = false;
					m_calibrationState = CalibrationState::CalibratingRightFist;
				}
			}
			break;

		case CalibrationState::CalibratingRightFist:
			if (!isLeft)
			{
				if (m_calibrationManager.collectFistSamples(sideKEY, fingerRaw))
				{
					
					std::cout << "Right fist done\n";
					m_calibrationManager.finishCalibration();
					m_calibrationState = CalibrationState::Idle;
					m_collecting = false;
					std::cout << "Calibration finished\n";
				}
			}
			break;

		default:
			break;
		}

		return; // pocas kalibracie preskakujeme rozpoznavanie gest
	}

	// --- NORMALIZACIA - aplikacia kalibracnych hodnot ---
	auto normalizedValues = m_calibrationManager.normalize(sideKEY, fingerRaw);

	logFile << timestamp.str();
	for (float v : normalizedValues)
		logFile << ";" << v;
	logFile << "\n";
	logFile.flush();

	// --- MAPOVANIE PRSTOV PRE ROZPOZNAVANIE GEST ---
	// Senzory lavej ruky su v opacnom HW poradí oproti pravej - prehodíme ich
	std::array<float, 5> gestureValues{};

	if (isLeft) {
		gestureValues[0] = normalizedValues[4]; // thumb/palec
		gestureValues[1] = normalizedValues[3]; // index/ukazovak
		gestureValues[2] = normalizedValues[2]; // middle/prostrednik
		gestureValues[3] = normalizedValues[1]; // ring/prstenik
		gestureValues[4] = normalizedValues[0]; // pinky/malicek
	}
	else {
		// prava ruka - poradie zodpoveda definiciam gest
		gestureValues = normalizedValues;
	}

	
	if (m_calibrationManager.m_isCalibrating) {
		return;
	}

	// --- ROZPOZNAVANIE GEST ---
	if (isLeft) {
		if (!m_leftHandEnabled) {
			return;
		}

		if (auto Left_gesture = m_leftGestureRecognizer.recognize(gestureValues)) {
			std::string gesture = *Left_gesture;
			m_leftGestureRecognizer.reset(); // reset citacov po detekcii

			if (m_experimentMode && m_experiment.isRunning()) {
				m_experiment.processGesture("LEFT", gesture);
				if (!m_experiment.isRunning()) {
					stopExperiment();		// auto-zastavenie po poslednom geste
				}
			}
			else {
				handleLeftGesture(gesture);
			}
		}
	}
	else {

		if (auto Right_gesture = m_rightGestureRecognizer.recognize(gestureValues)) {
			std::string gesture = *Right_gesture;
			m_rightGestureRecognizer.reset();
			if (m_experimentMode && m_experiment.isRunning()) {
				m_experiment.processGesture("RIGHT", gesture);
				if (!m_experiment.isRunning()) {
					stopExperiment();
				}
			}
			else {
				handleRightGesture(gesture);
			}

		}

	}

}

std::array<float, 5> GloveValues::getNormalizedFingerValues(const std::vector<uint8_t>& values, GSdk::BoardTools::ExternalSensorAssembly assembly) {

	std::array<float, 5> fingerValues = { 0.0f,0.0f ,0.0f ,0.0f ,0.0f };
	size_t count = 0;

	for (int tag : assembly.tags()) {
		if (m_logOnlyBending && (tag % 2 == 0)) {
			continue;								// preskoc IMU senzory
		}
		int index = assembly.findIndex(tag) * 4;	// kazda float hodnota zabera 4 bajty v BT datach
		float val = 0.0f;

		if (index >= 0 && index + 3 < values.size()) {
			std::memcpy(&val, &values[index], sizeof(float));

		}

		fingerValues[count] = val;
		count++;
		if (count >= 5) break;						// nacitavame len prvy 5 senzorov (prsty)

	}
	return fingerValues;
}





void GloveValues::subscribe(const std::string& gloveName, std::shared_ptr<GSdk::Board::BoardPeripheral> board) {

	auto streamTimeslots = GSdk::Board::getEmptyStreamTimeslots(); 
	streamTimeslots.sensorsState = 6; //6 alebo GSdkBoardStreamTypeSensorsState - data ohybu prstov
	
	
	// Pre aktivaciu IMU streamu odkomentovat nasledujuci riadok:
	//streamTimeslots.taredQuaternion = 6; 
	

	if (!board->streamTimeslots().write(streamTimeslots)) {
		this->printError("[ " + gloveName + " ]" + "Unable to set stream timeslots");
		return;
	}

	// Urcenie strany rukavice pre callback na zaklade ID v nazve zariadenia
	GSdk::BoardTools::WearingPosition position =
		(gloveName.find("4305") != std::string::npos)
		? GSdk::BoardTools::WearingPosition::GSdkWearingPositionLeftGlove
		: GSdk::BoardTools::WearingPosition::GSdkWearingPositionRightGlove;

	// Registracia callbacku - volany asynchronne pri kazdom prichode BT datoveho paketu
	int streamID = board->streamReceived().connect([this, gloveName, position](const GSdk::Board::BoardStreamEventArgs& args) {

		if (args.streamType == GSdkBoardStreamTypeSensorsState) {
			auto value = args.bytes();

			auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
			std::tm tm = *std::localtime(&now);
			char timeStr[32];
			std::strftime(timeStr, sizeof(timeStr), "%H:%M:%S", &tm);

			logRawToCSV(gloveName, value, position);	// zapis surovych dat
			logToCSV(gloveName, value, position);		// normalizacia + rozpoznavanie gest

		}

		if (args.streamType == GSdkBoardStreamTypeTaredQuaternion) {
			auto quatArgs = static_cast<const GSdk::Board::BoardQuaternionfEventArgs&> (args);
			auto quatV = quatArgs.value();
			this->printInfo("Received tared quaternion " + getString(quatV));
		}
	});

	m_streamIDs[gloveName] = streamID;					// ulozenie ID pre neskorsi odpoj callbacku
	 
}

