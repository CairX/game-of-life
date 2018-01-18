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
}
