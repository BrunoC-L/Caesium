#pragma once
#include "../core/toCPP.hpp"
#include "typename_visitor.hpp"

struct typename_for_template_visitor : TypenameVisitor<typename_for_template_visitor> {
	using TypenameVisitor<typename_for_template_visitor>::operator();

	using R = std::string;

	R operator()(const NodeStructs::BaseTypename& t);
	R operator()(const NodeStructs::NamespacedTypename& t);
	R operator()(const NodeStructs::TemplatedTypename& t);
	R operator()(const NodeStructs::UnionTypename& t);
};

std::string typename_for_template(
	const auto& tn
) {
	return typename_for_template_visitor{ {} }(tn);
}
