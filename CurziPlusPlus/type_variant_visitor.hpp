#pragma once
#include "node_structs.h"

template <typename T>
struct TypeCategoryVisitor {
	template <typename Self>
	auto operator()(this Self&& self, const NodeStructs::TypeCategory& t) {
		return std::visit(
			[&](const auto& t) {
				return self(t);
			},
			t.value
		);
	}
};
