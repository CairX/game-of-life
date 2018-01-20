#include "stdafx.hpp"

#include <exception>
#include <iostream>
#include <memory>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>

#include "clock.hpp"
#include "grid.hpp"
#include "entity.hpp"
#include "glmw.hpp"
#include "string.hpp"
#include "systems.hpp"

static const unsigned int WINDOW_WIDTH = 960;
static const unsigned int WINDOW_HEIGHT = 960;
static const unsigned int VIEW_WIDTH = 960;
static const unsigned int VIEW_HEIGHT = 960;
static const unsigned int VIEW_X = static_cast<unsigned int>(std::floor((WINDOW_WIDTH - VIEW_WIDTH) * 0.5f));
static const unsigned int VIEW_Y = static_cast<unsigned int>(std::floor((WINDOW_HEIGHT - VIEW_HEIGHT) * 0.5f));

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
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
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
		WINDOW_WIDTH, WINDOW_HEIGHT,
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
		glViewport(VIEW_X, VIEW_Y, VIEW_WIDTH, VIEW_HEIGHT);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		auto file = glmw::get_file_contents("grids/main.txt");
		if (!file.has_value()) {
			throw std::exception("Failed to read file: grids/main.txt");
		}
		auto gridz = str::split(file.value(), '\n');
		const int columns = gridz.front().size();
		const int rows = gridz.size();

		const float size = static_cast<float>(std::max(columns, rows));
		const glm::mat4 projection = glm::ortho(0.0f, size, 0.0f, size, 0.0f, 100.0f);
		std::vector<std::shared_ptr<ent::entity>> entities = grid::generate(columns, rows, gridz);

		sys::neighbours neighbours;
		sys::life life;
		sys::visuals visuals;
		rndr::world renderer(projection);
		clk::clock clock(std::chrono::milliseconds(1000 / 60));

		while (!glfwWindowShouldClose(window)) {
			clock.sleep();
			const float delta = clock.tick().count() * 0.001f;
			const float runtime = clock.runtime().count() * 0.001f;
			input(window);

			visuals.update(entities, delta, runtime);
			renderer.render(entities);
			neighbours.update(entities, delta, runtime);
			life.update(entities, delta, runtime);

			glfwSwapBuffers(window);
		}
	} catch (std::exception exception) {
		std::cerr << exception.what() << std::endl;
		system("pause");
	}

	glfwTerminate();
	return 0;
}
