#pragma once
#include "../core/toCPP.hpp"
#include "typename_visitor.hpp"

struct typename_original_representation_visitor : TypenameVisitor<typename_original_representation_visitor> {
	using TypenameVisitor<typename_original_representation_visitor>::operator();

	using R = std::string;

	R operator()(const NodeStructs::BaseTypename& t);
	R operator()(const NodeStructs::NamespacedTypename& t);
	R operator()(const NodeStructs::TemplatedTypename& t);
	R operator()(const NodeStructs::UnionTypename& t);
};

std::string typename_original_representation(
	const auto& tn
) {
	return typename_original_representation_visitor{ {} }(tn);
}
