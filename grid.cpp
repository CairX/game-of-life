#include "stdafx.hpp"
#include "grid.hpp"

#include <array>

std::shared_ptr<cmp::render> grid::generate_quad_buffer() {
	const GLfloat vertices[] = {
		0.5f,  0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f,
		-0.5f,  0.5f, 0.0f
	};
	const GLuint indices[] = { 0, 1, 3, 1, 2, 3 };

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

	return std::make_shared<cmp::render>(vao, 6);
}

std::vector<std::shared_ptr<ent::entity>> grid::generate(
	const int columns, const int rows,
	const std::vector<std::string> &data) {

	const float column_multiplier = 1.0f / columns;
	const float row_multiplier = 1.0f / rows;

	std::vector<std::shared_ptr<ent::entity>> entities;
	std::vector<std::vector<std::shared_ptr<cmp::cell>>> grid;

	const auto quad = generate_quad_buffer();

	// Generate entities
	for (int x = 0; x < columns; x++) {
		grid.push_back(std::vector<std::shared_ptr<cmp::cell>>(rows));
		for (int y = 0; y < rows; y++) {
			const glm::vec3 position(x + 0.5f, y + 0.5f, 0.0f);
			const glm::vec3 color(x * column_multiplier, y * row_multiplier, 1.0f);

			auto e = std::make_shared<ent::entity>();
			e->add<cmp::transform>(std::make_shared<cmp::transform>(position));
			e->add<cmp::render>(quad);
			e->add<cmp::visual>(std::make_shared<cmp::visual>(color));
			const auto cell = std::make_shared<cmp::cell>(data[rows - 1 - y][x] == '#');
			e->add<cmp::cell>(cell);

			entities.push_back(e);
			grid[x][y] = cell;
		}
	}

	// Associate neighbours
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

	return entities;
}
