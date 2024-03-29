#pragma once
#include "../core/toCPP.hpp"
#include "type_category_visitor.hpp"

struct traverse_type_visitor : TypeCategoryVisitor<traverse_type_visitor> {
	using TypeCategoryVisitor<traverse_type_visitor>::operator();

	transpilation_state_with_indent state;

	using R = std::optional<error>;

	TypeCategoryVisitorDeclarations
};

std::optional<error> traverse_type(
	transpilation_state_with_indent state,
	const auto& type
) {
	return traverse_type_visitor{ {}, state }(type);
}
