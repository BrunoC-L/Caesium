#pragma once
#include "node_structs.hpp"

template <typename T>
concept TypenameVisitorConcept = requires(T && t, const NodeStructs::Typename & v) {
	t(v);
	std::visit(t, v.value);
};

template <typename T>
struct TypenameVisitor {
	template <typename Self>
	auto operator()(this Self&& self, const NodeStructs::Typename& t) {
		return std::visit(
			[&](const auto& t) {
				return self(t);
			},
			t.value
		);
	}
	/*
	R operator()(const NodeStructs::BaseTypename& t);
	R operator()(const NodeStructs::NamespacedTypename& t);
	R operator()(const NodeStructs::TemplatedTypename& t);
	R operator()(const NodeStructs::UnionTypename& t);
	*/
};
