#pragma once

#include <memory>
#include <string>
#include <vector>
#include "entity.hpp"

namespace grid {
	std::shared_ptr<cmp::render> generate_quad_buffer();
	std::vector<std::shared_ptr<ent::entity>> generate(int columns, int rows, const std::vector<std::string> &data);
}
