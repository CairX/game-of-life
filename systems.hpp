#pragma once

#include <memory>
#include <vector>

#include "entity.hpp"

namespace sys {
	class system {
	public:
		virtual ~system() = default;
		virtual void update(std::vector<std::shared_ptr<ent::entity>> &entities, float delta, float runtime) = 0;
	};

	class neighbours : public system {
	public:
		neighbours() : neighbours(1.0f) {}
		neighbours(const float interval) : interval(interval), timer(0.0f) {}
		void update(std::vector<std::shared_ptr<ent::entity>> &entities, float delta, float runtime) override;
	private:
		float interval;
		float timer;
	};

	class life : public system {
	public:
		life() : life(1.0f) {}
		life(const float interval) : interval(interval), timer(0.0f) {}
		void update(std::vector<std::shared_ptr<ent::entity>> &entities, float delta, float runtime) override;

	private:
		float interval;
		float timer;
	};

	class visuals : public system {
	public:
		void update(std::vector<std::shared_ptr<ent::entity>> &entities, float delta, float runtime) override;
	};
}

namespace rndr {
	class renderer {
	public:
		virtual ~renderer() = default;
		virtual void render(std::vector<std::shared_ptr<ent::entity>> &entities) = 0;
	};

	class world : public renderer {
	public:
		world(glm::mat4 projection);
		void render(std::vector<std::shared_ptr<ent::entity>> &entities) override;

	private:
		GLuint program;
	};
}
