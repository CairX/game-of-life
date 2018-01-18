#include "stdafx.hpp"
#include "glmw.hpp"

std::optional<std::string> glmw::get_file_contents(const std::string &path) {
	std::ifstream file(path, std::ios::in | std::ios::binary);
	if (file) {
		std::string contents;
		file.seekg(0, std::ios::end);
		contents.resize(file.tellg());
		file.seekg(0, std::ios::beg);
		file.read(&contents[0], contents.size());
		file.close();
		return std::make_optional(contents);
	}
	return std::nullopt;
}

GLuint glmw::create_shader(const GLenum type, const std::string &path) {
	const GLuint shader = glCreateShader(type);
	std::optional<std::string> shader_content = get_file_contents(path);
	if (shader_content) {
		std::clog << "Shader file loaded: " << path << std::endl;
	} else {
		throw std::exception(("Shader file failed to load: " + path).c_str());
	}

	const GLchar *source = shader_content.value().c_str();
	glShaderSource(shader, 1, &source, nullptr);
	glCompileShader(shader);

	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_TRUE) {
		std::clog << "Shader compiled: " << path << std::endl;
	} else {
		GLsizei log_length = 0;
		GLchar message[1024];
		glGetShaderInfoLog(shader, 1024, &log_length, message);
		throw std::exception(message);
	}

	return shader;
}

GLuint glmw::create_program(const std::string &vertex_path, const std::string &fragment_path) {
	const GLuint vertex_shader = create_shader(GL_VERTEX_SHADER, vertex_path);
	const GLuint fragment_shader = create_shader(GL_FRAGMENT_SHADER, fragment_path);

	const GLuint program = glCreateProgram();
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
	glLinkProgram(program);

	GLint program_status;
	glGetProgramiv(program, GL_LINK_STATUS, &program_status);
	if (program_status == GL_TRUE) {
		std::clog << "Shader program composed." << std::endl;
	} else {
		throw std::exception("Error compiling shader program.");
	}

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	return program;
}
