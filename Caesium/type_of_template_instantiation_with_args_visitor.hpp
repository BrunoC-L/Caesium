#pragma once
#include "toCPP.hpp"
#include "type_category_visitor.hpp"

struct type_of_template_instantiation_with_args_visitor : TypeCategoryVisitor<type_of_template_instantiation_with_args_visitor> {
	using TypeCategoryVisitor<type_of_template_instantiation_with_args_visitor>::operator();

	transpilation_state_with_indent state;
	const std::vector<NodeStructs::Expression>& templated_with;

	using R = expected<NodeStructs::UniversalType>;

	TypeCategoryVisitorDeclarations
};
