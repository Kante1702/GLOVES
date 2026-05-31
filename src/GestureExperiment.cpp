// GestureExperiment.cpp
// Implementacia experimentalnej verifikacie rozpoznavania gest.


#include "GestureExperiment.hpp"
#include <conio.h>
#include <iostream>
#include <thread>
#include <random>





void GestureExperiment::buildRandomSequence() {
	m_sequence.clear();

	std::random_device rd;
	std::mt19937 rng(rd());

	// Rozdelenie gest podla ruky
	std::vector<std::pair<std::string, std::string>> leftGestures, rightGestures;
	for (const auto& g : m_gestures) {
		if (g.first == "LEFT") {
			leftGestures.push_back(g);
		}
		else {
			rightGestures.push_back(g);

		}
	}

	// Naplnenie bazenov - kazde gesto sa opakuje m_repeatCount krat
	std::vector<std::pair<std::string, std::string >> leftPool, RightPool;
	for (int i = 0; i < m_repeatCount; i++) {
		for (const auto& g : leftGestures) {
			leftPool.push_back(g);
		}
		for (const auto& g : rightGestures) {
			RightPool.push_back(g);
		}
	}

	// Nahodne zamieshanie v ramci kazdeho bazena
	std::shuffle(leftPool.begin(), leftPool.end(), rng);
	std::shuffle(RightPool.begin(), RightPool.end(), rng);

	// Striedanie lavej a pravej ruky v sekvencii - znizuje pocet faloshnych
	// zaznamov v matici zamen, pretoze gesta z neocakavanej ruky su ignorovane
	size_t li = 0;
	size_t ri = 0;

	while (li < leftPool.size() || ri < RightPool.size()) {
		if (li < leftPool.size()) {
			m_sequence.push_back(leftPool[li]);
			li += 1;
		}
		if (ri < RightPool.size()) {
			m_sequence.push_back(RightPool[ri]);
			ri += 1;
		}
	}

	std::cout << "[Experiment] Sequence built: " << m_sequence.size()<< " gestures (" << m_gestures.size() << " unique x "<< m_repeatCount << " repeats)\n";
}

void GestureExperiment::start()
{
	m_results.clear();
	m_resultCounts.clear();
	m_confusionMatrix.clear();
	m_waitingForStart = false;
	m_running = true;


	if (m_file.is_open()) {
		m_file.close();
	}

	// Nazov suboru obsahuje cas spustenia pre jednoznacnu identifikaciu merania
	auto now = std::chrono::system_clock::now();
	std::time_t t = std::chrono::system_clock::to_time_t(now);
	std::tm tm{};
	localtime_s(&tm, &t);

	char filename[64];
	std::strftime(filename, sizeof(filename), "ExperimentResults_%H%M%S.csv", &tm);
	m_file.open(filename);
	m_file << "Ruka" << ";" << "Gesto" <<";" << "ReakcnyCas[ms]\n";

	
	std::cout << "\nExperiment starting in:\n";
	for (int i = 3; i > 0; i--) {
		std::cout << i << "...\n";
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	buildRandomSequence();
	m_currentIndex = 0;
	showNextGesture();
}


void GestureExperiment::stopEarly() {
	if (!m_running) {
		return;
	}

	m_running = false;
	if (m_file.is_open()) {
		m_file.close();
	}

	std::cout << "\n[Experiment] Stopped early\n";
	saveAverages();
	saveConfusionMatrix();

}





void GestureExperiment::setGestures(const std::vector<std::pair<std::string, std::string>>& gestures) {
	m_gestures = gestures;
}

void GestureExperiment::showNextGesture() {
	m_gestureAccepted = false; //odblokuj prijem gest pre nove gesto

	if (m_currentIndex >= m_sequence.size()) {
		std::cout << "\n==================================\n";
		std::cout << "Experiment finished\n";
		std::cout << "Total gestures: " << m_sequence.size() << "\n";
		std::cout << "==================================\n";
		m_running = false;
		m_file.close();
		saveAverages();
		saveConfusionMatrix();
		return;
	}


	// Pauza medzi gestami - nezapocitava sa do reakcneho casu, ale zabranuje
   // tomu, aby koniec predchadzajuceho gesta ovplyvnil meranie nasledujuceho
	std::this_thread::sleep_for(std::chrono::milliseconds(m_delayBetweenGesturesMs));

	auto g =m_sequence[m_currentIndex];

	std::cout << "\n--------------------------------\n";
	std::cout << "Perform gesture:\n";
	std::cout << g.first << " -> " << g.second << "\n";
	std::cout << "--------------------------------\n";

	// Meranie reakcneho casu zacina az po zobrazeni vyzvy
	m_startTime = std::chrono::steady_clock::now();

}

void GestureExperiment::saveAverages() {
	

	auto now = std::chrono::system_clock::now();
	std::time_t t = std::chrono::system_clock::to_time_t(now);
	std::tm tm{};
	localtime_s(&tm, &t);
	char filename[64];
	std::strftime(filename, sizeof(filename), "ExperimentAverages_%H%M%S.csv", &tm);

	std::ofstream avgFile(filename);
	avgFile << "Gesto" <<";" << "Priemer[ms]" << ";" << "Pocet\n";
	for (auto& [gesture, sum] : m_results) {
		int count = m_resultCounts[gesture];
		double avg = (double)sum / count;
		avgFile << gesture << ";" << avg << ";" << count << "\n";

	}

	avgFile.close();

}

void GestureExperiment::saveConfusionMatrix() {

	auto now = std::chrono::system_clock::now();
	std::time_t t = std::chrono::system_clock::to_time_t(now);
	std::tm tm{};
	localtime_s(&tm, &t);
	char filename[64];
	std::strftime(filename, sizeof(filename), "ConfusionMatrix_%H%M%S.csv", &tm);

	// Pouzivame poradie gest zo vstupneho zoznamu pre konzistentne oznacenie riadkov a stlpcov
	std::vector<std::string> labels;
	for (const auto& g : m_gestures) {
		labels.push_back(g.second);
	}

	std::ofstream f(filename);

	// Hlavicka - riadok = ocakavane gesto, stlpec = rozoznane gesto
	f << "Ocakavane (riadok)\\ Rozoznane (stlpec)";
	for (const auto& col : labels) {
		f << ";" << col;
	}
	f << "\n";

	// Riadky matice - kazda bunka obsahuje pocet zamen
	for (const auto& row : labels) {

		f << row;
		for (const auto& col : labels) {
			int count = 0;
			auto it = m_confusionMatrix.find(row);
			if (it != m_confusionMatrix.end()) {
				auto it2 = it->second.find(col);
				if (it2 != it->second.end()) {
					count = it2->second;
				}
			}
			f << ";" << count;
		}
		f << "\n";
	}
	f.close();
}


void GestureExperiment::processGesture(const std::string& hand, const std::string& gesture) {

	if (!m_running) {
		return;
	}

	if (m_currentIndex >= m_sequence.size()) {
		return;
	}

	auto expected = m_sequence[m_currentIndex];
	
	// Gesta z neocakavanej ruky su uplne ignorovane - striedanie ruk zaistuje,
	// ze jedno gesto nemozno splnit za ine z opacnej ruky
	if (expected.first != hand) {
		return;
	}

	//ak uz bolo gesto rozpoznane, ignoruj dalsie callbacky z rukavic
	if (m_gestureAccepted) {
		return;
	}

	auto end = std::chrono::steady_clock::now();
	
	// Reakcny cas sa meria pre vsetky gesta spravnej ruky, aj pri nespravenej zamene
	auto reactionTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - m_startTime).count();

	// Matica zamen sa aktualizuje pre spravne aj nespravne gesta (vratan diagonala)
	m_confusionMatrix[expected.second][gesture]++;
	
	if (expected.second != gesture) {
		// Nespravne gesto - zaznamenana zamena, cakame na spravne gesto
		std::cout << "[!!!] Wrong: expected " << expected.second << "\n";
		return;
	}

	// Spravne gesto - ulozenie reakcneho casu a posun na dalsi prvok sekvencie
	std::cout << "Recognized in: " << reactionTime << " ms" << std::endl;
	m_file << hand << ";" << gesture << ";" << reactionTime << "\n";
	m_results[gesture] += reactionTime;
	m_resultCounts[gesture]++;
	m_file.flush();				// okamzity zapis - ochrana pred stratou dat pri predcasnom ukonceni
	m_gestureAccepted = true;	//zamedzenie zapisovaniu dodatocnych roroznavani po uspesnom geste

	m_currentIndex++;
	showNextGesture();
}

