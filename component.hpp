#pragma once
#include <memory>
#include <vector>

namespace cmp {
	class component {};

	class transform : public component {
	public:
		glm::vec3 position;

		transform(const glm::vec3 position) :
			position(position) {}
	};

	class render : public component {
	public:
		GLuint vao;
		GLsizei count;

		render(const GLuint vao, const GLsizei count) :
			vao(vao), count(count) {}
	};

	class visual : public component {
	public:
		glm::vec3 color;

		visual(const glm::vec3 color) :
			color(color) {}
	};

	class cell : public component {
	public:
		bool previously = false;
		bool alive;
		std::vector<std::shared_ptr<cell>> neighbours;

		cell() : alive(false) {}
		cell(const bool alive) : alive(alive) {}
	};
}
