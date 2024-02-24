#pragma once
#include "../core/toCPP.hpp"
#include "expression_visitor.hpp"

struct transpile_call_expression_with_args_visitor : ExpressionVisitor<transpile_call_expression_with_args_visitor> {
	using ExpressionVisitor<transpile_call_expression_with_args_visitor>::operator();

	transpilation_state_with_indent state;

	const std::vector<NodeStructs::FunctionArgument>& arguments;

	using R = transpile_t;

	ExpressionVisitorDeclarations
};
