#pragma once
#include "../core/toCPP.hpp"
#include "type_category_visitor.hpp"

struct type_of_function_like_call_with_args_visitor : TypeCategoryVisitor<type_of_function_like_call_with_args_visitor> {
	using TypeCategoryVisitor<type_of_function_like_call_with_args_visitor>::operator();

	transpilation_state_with_indent state;
	const std::vector<NodeStructs::FunctionArgument>& args;

	using R = expected<std::pair<NodeStructs::ParameterCategory, NodeStructs::UniversalType>>;

	TypeCategoryVisitorDeclarations
};
