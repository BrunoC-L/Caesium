#pragma once
#include "../core/toCPP.hpp"
#include "typename_visitor.hpp"

struct type_of_typename_visitor : TypenameVisitor<type_of_typename_visitor> {
	using TypenameVisitor<type_of_typename_visitor>::operator();

	transpilation_state_with_indent state;

	using R = expected<NodeStructs::MetaType>;

	R operator()(const NodeStructs::BaseTypename& t);
	R operator()(const NodeStructs::NamespacedTypename& t);
	R operator()(const NodeStructs::TemplatedTypename& t);
	R operator()(const NodeStructs::UnionTypename& t);
};

expected<NodeStructs::MetaType> type_of_typename(
	transpilation_state_with_indent state,
	const auto& tn
) {
	return type_of_typename_visitor{ {}, state }(tn);
}
