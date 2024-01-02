#pragma once
#include "toCPP.hpp"
#include "typename_visitor.hpp"

struct type_of_typename_visitor : TypenameVisitor<type_of_typename_visitor> {
	using TypenameVisitor<type_of_typename_visitor>::operator();

	transpilation_state& state;

	using R = std::expected<NodeStructs::TypeCategory, user_error>;

	R operator()(const NodeStructs::BaseTypename& t);
	R operator()(const NodeStructs::NamespacedTypename& t);
	R operator()(const NodeStructs::TemplatedTypename& t);
	R operator()(const NodeStructs::UnionTypename& t);
};
