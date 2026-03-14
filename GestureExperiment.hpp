#pragma once
#ifndef GESTUREEXPERIMENT_HPP
#define GESTUREEXPERIMENT_HPP

#include <vector>
#include <string>
#include <fstream>
#include <chrono>
#include <random>
#include <utility>
#include <unordered_map>

class GestureExperiment {

    bool m_waitingForStart = true;
    bool m_running = false;

    size_t m_currentIndex = 0;
    int m_repeatCount = 5; //pocet opakovani gesta
    std::vector<std::pair<std::string, std::string>> m_gestures; //vstupne gesta
    std::vector<std::pair<std::string, std::string>> m_sequence; //nahodne poradie 
    std::unordered_map<std::string, long long> m_results;
    std::unordered_map<std::string, int> m_resultCounts;
    std::chrono::steady_clock::time_point m_startTime;
    std::ofstream m_file;

    
    void buildRandomSequence();
    void showNextGesture();
    void saveAverages();


public: 

    void start();
	void processGesture(const std::string& hand, const std::string& gesture);
	void setGestures(const std::vector<std::pair<std::string, std::string>>& gestures);
	bool isRunning() const { return m_running; }
};




#endif // !GESTUREEXPERIMENT_HPP

