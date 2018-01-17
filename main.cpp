#include "stdafx.hpp"

#include <exception>
#include <iostream>

void error_glfw(int error, const char* description) {
	std::cerr << error << ": " << description << std::endl;
}

void initiate_glfw() {
	if (glfwInit()) {
		std::clog << "GLFW was initiated." << std::endl;
	} else {
		throw std::exception("GLFW failed to initiate.");
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

void initiate_glad() {
	if (gladLoadGLLoader(GLADloadproc(glfwGetProcAddress))) {
		std::clog << "GLAD was initiated." << std::endl;
	} else {
		throw std::exception("GLAD failed to initiate.");
	}
}

GLFWwindow* create_window() {
	GLFWwindow *window = glfwCreateWindow(
		1920, 1080,
		"Game of Life",
		nullptr, nullptr);

	if (window != nullptr) {
		std::clog << "GLFW created a window." << std::endl;
	} else {
		throw std::exception("GLFW failed to create a window.");
	}
	glfwMakeContextCurrent(window);
	return window;
}

void input(GLFWwindow *window) {
	glfwPollEvents();
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}

int main(int argc, char *argv[]) {
	glfwSetErrorCallback(error_glfw);

	try {
		initiate_glfw();
		GLFWwindow *window = create_window();
		initiate_glad();

		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		glViewport(0, 0, 1920, 1080);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		while (!glfwWindowShouldClose(window)) {
			input(window);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glfwSwapBuffers(window);
		}
	} catch (std::exception exception) {
		std::cerr << exception.what() << std::endl;
		system("pause");
	}

	glfwTerminate();
	return 0;
}
