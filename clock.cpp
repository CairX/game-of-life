#include "stdafx.hpp"
#include "clock.hpp"
#include <thread>

std::chrono::milliseconds clk::clock::tick() {
	previous = now;
	now = std::chrono::steady_clock::now();
	delta = std::chrono::duration_cast<std::chrono::milliseconds>(now - previous);

	return delta;
}

std::chrono::milliseconds clk::clock::runtime() const {
	return std::chrono::duration_cast<std::chrono::milliseconds>(now - start);
}

void clk::clock::sleep() const {
	std::this_thread::sleep_for(fps - delta);
}
