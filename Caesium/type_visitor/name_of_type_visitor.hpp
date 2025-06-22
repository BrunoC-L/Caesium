#pragma once
#include "../core/toCPP.hpp"
#include "type_category_visitor.hpp"

struct name_of_type_visitor : TypeCategoryVisitor<name_of_type_visitor> {
	using TypeCategoryVisitor<name_of_type_visitor>::operator();

	transpilation_state_with_indent state;

	using R = transpile_t;

	TypeCategoryVisitorDeclarations
};

auto name_of_type(
	transpilation_state_with_indent state,
	const auto& type
) {
	return name_of_type_visitor{ {}, state }(type);
}
