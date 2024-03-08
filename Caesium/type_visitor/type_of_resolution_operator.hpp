#pragma once
#include "../core/toCPP.hpp"
#include "type_category_visitor.hpp"

struct type_of_resolution_operator_visitor : TypeCategoryVisitor<type_of_resolution_operator_visitor> {
	using TypeCategoryVisitor<type_of_resolution_operator_visitor>::operator();

	transpilation_state_with_indent state;
	const std::string& accessed;

	using R = expected<NodeStructs::UniversalType>;

	TypeCategoryVisitorDeclarations
};

expected<NodeStructs::UniversalType> type_of_resolution_operator(
	transpilation_state_with_indent state,
	const auto& type,
	const std::string& accessed
) {
	return type_of_resolution_operator_visitor{ {}, state, accessed }(type);
}
