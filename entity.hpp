#pragma once
#include <memory>
#include <typeindex>
#include <unordered_map>

#include "component.hpp"

namespace ent {
	class entity {
	public:
		template<typename T>
		std::shared_ptr<T> get() {
			auto it = components.find(std::type_index(typeid(T)));
			if (it != components.end()) {
				return std::static_pointer_cast<T>(it->second);
			}
			return nullptr;
		}

		template<typename T>
		void add(const std::shared_ptr<cmp::component> &c) {
			components[std::type_index(typeid(T))] = c;
		}

	private:
		std::unordered_map<std::type_index, std::shared_ptr<cmp::component>> components;
	};	
}
