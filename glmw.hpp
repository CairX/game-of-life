#pragma once
#include "stdafx.hpp"

#include <exception>
#include <fstream>
#include <iostream>
#include <optional>
#include <string>

namespace glmw {

	std::optional<std::string> get_file_contents(const std::string &path);

	GLuint create_shader(const GLenum type, const std::string &path);

	GLuint create_program(const std::string &vertex_path, const std::string &fragment_path);
}
