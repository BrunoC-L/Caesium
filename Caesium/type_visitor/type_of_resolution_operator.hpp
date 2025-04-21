#pragma once
#include "../core/toCPP.hpp"
#include "type_category_visitor.hpp"

struct type_of_resolution_operator_visitor : TypeCategoryVisitor<type_of_resolution_operator_visitor> {
	using TypeCategoryVisitor<type_of_resolution_operator_visitor>::operator();

	transpilation_state_with_indent state;
	variables_t& variables;
	const std::string& accessed;

	using R = expected<Realised::MetaType>;

	TypeCategoryVisitorDeclarations
};

expected<Realised::MetaType> type_of_resolution_operator(
	transpilation_state_with_indent state,
	variables_t& variables,
	const auto& type,
	const std::string& accessed
) {
	return type_of_resolution_operator_visitor{ {}, state, variables, accessed }(type);
}
