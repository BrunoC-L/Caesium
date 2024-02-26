#pragma once
#include "../core/toCPP.hpp"
#include "type_category_visitor.hpp"

struct transpile_type_visitor : TypeCategoryVisitor<transpile_type_visitor> {
	using TypeCategoryVisitor<transpile_type_visitor>::operator();

	transpilation_state_with_indent state;

	using R = transpile_t;

	TypeCategoryVisitorDeclarations
};

transpile_t transpile_type(
	transpilation_state_with_indent state,
	const auto& type
) {
	return transpile_type_visitor{ {}, state }(type);
}
