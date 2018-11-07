#pragma once

#include <chrono>

class Timer {
	std::chrono::high_resolution_clock::time_point time1 = std::chrono::high_resolution_clock::now();
	std::chrono::high_resolution_clock::time_point time2 = std::chrono::high_resolution_clock::now();

	int current_frame = 0;
	bool is_frame_new = true;

	int frame_duration;

public:

	Timer(int frame_duration) : frame_duration(frame_duration) {}

	void processTime() {

	}
};