#include "stdafx.hpp"

#include <array>
#include <exception>
#include <iostream>
#include <memory>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "clock.hpp"
#include "entity.hpp"
#include "glmw.hpp"
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

		glm::mat4 projection = glm::ortho(0.0f, 10.0f, 0.0f, 10.0f, 0.0f, 100.0f);

		GLfloat vertices[] = {
			0.5f,  0.5f, 0.0f,
			0.5f, -0.5f, 0.0f,
			-0.5f, -0.5f, 0.0f,
			-0.5f,  0.5f, 0.0f
		};
		GLuint indices[] = { 0, 1, 3, 1, 2, 3 };

		const GLuint program = glmw::create_program("assets/shaders/basic.vs", "assets/shaders/basic.fs");
		glUseProgram(program);
		glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		GLuint vao;
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		GLuint vbo;
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GL_FLOAT), vertices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

		GLuint ebo;
		glGenBuffers(1, &ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		std::vector<std::shared_ptr<ent::entity>> entities;
		const int columns = 10;
		const int rows = 10;
		const float column_multiplier = 1.0f / columns;
		const float row_multiplier = 1.0f / rows;
		std::array<std::array<std::shared_ptr<cmp::cell>, rows>, columns> grid;

		for (int x = 0; x < columns; x++) {
			for (int y = 0; y < rows; y++) {
				const float position_x = x + 0.5f;
				const float position_y = y + 0.5f;
				auto e = std::make_shared<ent::entity>();
				e->add<cmp::transform>(std::make_shared<cmp::transform>(glm::vec3(position_x, position_y, 0.0f)));
				e->add<cmp::render>(std::make_shared<cmp::render>(vao, 6));
				e->add<cmp::visual>(std::make_shared<cmp::visual>(glm::vec3(x * column_multiplier, y * row_multiplier, 1.0f)));
				const auto cell = std::make_shared<cmp::cell>();
				e->add<cmp::cell>(cell);
				entities.push_back(e);
				grid[x][y] = cell;
			}
		}

		const std::vector<std::pair<int, int>> directions{
			{ -1,  1 }, { 0,  1 }, { 1,  1 },
			{ -1,  0 },            { 1,  0 },
			{ -1, -1 }, { 0, -1 }, { 1, -1 }
		};
		for (int x = 0; x < columns; x++) {
			for (int y = 0; y < rows; y++) {
				std::shared_ptr<cmp::cell> cell = grid[x][y];
				for (const auto direction : directions) {
					const int neighbour_x = x + direction.first;
					const int neighbour_y = y + direction.second;

					if (neighbour_x < 0 || neighbour_x >= columns) { continue; }
					if (neighbour_y < 0 || neighbour_y >= rows) { continue; }

					cell->neighbours.push_back(grid[neighbour_x][neighbour_y]);
				}
			}
		}

		const std::vector<std::pair<int, int>> blinker{
			{ 5, 4 }, { 5, 5 }, { 5, 6 },
		};
		for (const auto position : blinker) {
			std::shared_ptr<cmp::cell> cell = grid[position.first][position.second];
			cell->alive = true;
		}

		sys::life life;
		sys::visuals visuals;
		clk::clock clock(std::chrono::milliseconds(1000 / 60));

		while (!glfwWindowShouldClose(window)) {
			clock.sleep();
			const float delta = clock.tick().count() * 0.001f;
			const float runtime = clock.runtime().count() * 0.001f;
			input(window);

			life.update(entities, delta, runtime);
			visuals.update(entities, delta, runtime);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glUseProgram(program);
			for (std::shared_ptr<ent::entity> e : entities) {
				const auto transform = e->get<cmp::transform>();
				const auto render = e->get<cmp::render>();
				const auto visual = e->get<cmp::visual>();
				glBindVertexArray(render->vao);
				glm::mat4 model = glm::translate(glm::mat4(1.0f), transform->position);
				glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(model));
				glUniform3fv(glGetUniformLocation(program, "color"), 1, glm::value_ptr(visual->color));
				glDrawElements(GL_TRIANGLES, render->count, GL_UNSIGNED_INT, 0);
			}
			glfwSwapBuffers(window);
		}
	} catch (std::exception exception) {
		std::cerr << exception.what() << std::endl;
		system("pause");
	}

	glfwTerminate();
	return 0;
}
