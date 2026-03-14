#include "GestureExperiment.hpp"
#include <conio.h>
#include <iostream>
#include <thread>
#include <random>





void GestureExperiment::buildRandomSequence() {
	m_sequence.clear();

	std::random_device rd;
	std::mt19937 rng(rd());

	for (int i = 0; i < m_repeatCount; i++)
		for (const auto& g : m_gestures)
			m_sequence.push_back(g);

	bool hasDuplicates = true;
	while (hasDuplicates) {
		std::shuffle(m_sequence.begin(), m_sequence.end(), rng);
		hasDuplicates = false;
		for (size_t i = 1; i < m_sequence.size(); i++) {
			if (m_sequence[i].second == m_sequence[i - 1].second) {
				hasDuplicates = true;
				break;
			}
		}
	}

	std::cout << "[Experiment] Sequence built: " << m_sequence.size()<< " gestures (" << m_gestures.size() << " unique x "<< m_repeatCount << " repeats)\n";
}

void GestureExperiment::start()
{
	m_results.clear();
	m_resultCounts.clear();
	m_waitingForStart = false;
	m_running = true;


	if (m_file.is_open()) {
		m_file.close();
	}


	auto now = std::chrono::system_clock::now();
	std::time_t t = std::chrono::system_clock::to_time_t(now);
	std::tm tm{};
	localtime_s(&tm, &t);
	char filename[64];
	std::strftime(filename, sizeof(filename), "ExperimentResults_%H%M%S.csv", &tm);
	
	m_file.open(filename);
	m_file << "Ruka" << ";" << "Gesto" <<";" << "ReakcnyÈas[ms]\n";

	std::cout << "\nExperiment starting in:\n";
	for (int i = 3; i > 0; i--) {
		std::cout << i << "...\n";
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	buildRandomSequence();
	m_currentIndex = 0;
	showNextGesture();
}

void GestureExperiment::setGestures(const std::vector<std::pair<std::string, std::string>>& gestures) {
	m_gestures = gestures;
}

void GestureExperiment::showNextGesture() {
	
	if (m_currentIndex >= m_sequence.size()) {
		std::cout << "\n==================================\n";
		std::cout << "Experiment finished\n";
		std::cout << "Total gestures: " << m_sequence.size() << "\n";
		std::cout << "==================================\n";
		m_running = false;
		m_file.close();
		saveAverages();
		return;
	}

	auto g =m_sequence[m_currentIndex];

	std::cout << "\n--------------------------------\n";
	std::cout << "Perform gesture:\n";
	std::cout << g.first << " -> " << g.second << "\n";
	std::cout << "--------------------------------\n";

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


void GestureExperiment::processGesture(const std::string& hand, const std::string& gesture) {

	if (!m_running) {
		return;
	}

	auto expected = m_sequence[m_currentIndex];
	if (expected.first != hand) {
		return;
	}
	if (expected.second != gesture) {
		return;
	}

	auto end = std::chrono::steady_clock::now();

	auto reactionTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - m_startTime).count();

	std::cout << "Recognized in: " << reactionTime << " ms" << std::endl;
	m_file << hand << ";" << gesture << ";" << reactionTime << "\n";
	m_results[gesture] += reactionTime;
	m_resultCounts[gesture]++;
	m_file.flush();
	m_currentIndex++;
	showNextGesture();
}

