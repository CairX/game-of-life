#include "stdafx.hpp"
#include "systems.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "glmw.hpp"

void sys::visuals::update(std::vector<std::shared_ptr<ent::entity>> &entities, const float delta, const float runtime) {
	for (auto entity : entities) {
		auto visual = entity->get<cmp::visual>();
		if (visual == nullptr) { continue; }
		const auto cell = entity->get<cmp::cell>();
		if (cell == nullptr) { continue; }

		if (cell->alive) {
			visual->color = glm::vec3(1.0f);
		} else {
			visual->color = glm::vec3(0.2f);
		}
	}
}

void sys::life::update(std::vector<std::shared_ptr<ent::entity>> &entities, const float delta, const float runtime) {
	timer += delta;
	if (timer < interval) {return; }

	timer = 0.0f;
	std::vector<std::shared_ptr<cmp::cell>> cells;

	for (auto entity : entities) {
		const auto cell = entity->get<cmp::cell>();
		if (cell == nullptr) { continue; }

		cell->previously = cell->alive;
		cells.push_back(cell);
	}

	for (auto cell : cells) {
		unsigned int alive_neighbours = 0;
		for (const auto neighbour : cell->neighbours) {
			if (neighbour->previously) { alive_neighbours++; }
		}

		if (cell->previously) {
			// 1. Any live cell with fewer than two live neighbours dies, as if caused by underpopulation.
			if (alive_neighbours < 2) {
				cell->alive = false;
			}
			// 2. Any live cell with two or three live neighbours lives on to the next generation.
			else if (alive_neighbours == 2 || alive_neighbours == 3) {
				cell->alive = true;
			}
			// 3. Any live cell with more than three live neighbours dies, as if by overpopulation.
			else if (alive_neighbours > 3) {
				cell->alive = false;
			}
		} else {
			// 4. Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.
			cell->alive = (alive_neighbours == 3);
		}
	}
}

rndr::world::world(const glm::mat4 projection) {
	program = glmw::create_program("assets/shaders/basic.vs", "assets/shaders/basic.fs");
	glUseProgram(program);
	glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
}

void rndr::world::render(std::vector<std::shared_ptr<ent::entity>> &entities) {
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
}
