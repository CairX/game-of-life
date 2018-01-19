#pragma once

#include <chrono>

namespace clk{
	struct clock {
		std::chrono::steady_clock::time_point start;
		std::chrono::steady_clock::time_point previous;
		std::chrono::steady_clock::time_point now;
		std::chrono::milliseconds delta;
		std::chrono::milliseconds fps;

		clock(const std::chrono::milliseconds fps) :
			start{ std::chrono::steady_clock::now() }, now{ start }, fps{ fps } {
			tick();
		}

		std::chrono::milliseconds tick();

		std::chrono::milliseconds runtime() const;

		void sleep() const;
	};
}
