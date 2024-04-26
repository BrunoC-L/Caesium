#pragma once
#include "../core/toCPP.hpp"
#include "expression_visitor.hpp"

struct transpile_expression_visitor : ExpressionVisitor<transpile_expression_visitor> {
	using ExpressionVisitor<transpile_expression_visitor>::operator();

	transpilation_state_with_indent state;
	variables_t& variables;

	using R = transpile_t2;

	ExpressionVisitorDeclarations
};

transpile_t2 transpile_expression(
	transpilation_state_with_indent state,
	variables_t& variables,
	const auto& expr
) {
	return transpile_expression_visitor{ {}, state, variables }(expr);
}
