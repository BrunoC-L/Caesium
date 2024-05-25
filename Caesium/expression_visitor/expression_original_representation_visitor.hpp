#pragma once
#include "../core/toCPP.hpp"
#include "expression_visitor.hpp"

struct expression_original_representation_visitor : ExpressionVisitor<expression_original_representation_visitor> {
	using ExpressionVisitor<expression_original_representation_visitor>::operator();

	using R = std::string;
	ExpressionVisitorDeclarations
};

expression_original_representation_visitor::R expression_original_representation(
	const auto& expr
) {
	return expression_original_representation_visitor{ {} }(expr);
}
