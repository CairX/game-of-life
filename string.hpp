#pragma once
#include "stdafx.hpp"

#include <algorithm>
#include <locale>
#include <sstream>
#include <string>
#include <vector>

namespace str {
	void ltrim(std::string &s) {
		s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
			return !std::isspace(ch, std::locale());
		}));
	}

	void rtrim(std::string &s) {
		s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
			return !std::isspace(ch, std::locale());
		}).base(), s.end());
	}

	void trim(std::string &s) {
		ltrim(s);
		rtrim(s);
	}

	std::vector<std::string> split(const std::string &s, const char delimiter) {
		std::vector<std::string> parts;
		std::istringstream stream(s);
		for (std::string part; std::getline(stream, part, delimiter); ) {
			parts.push_back(part);
		}

		return parts;
	}
}
