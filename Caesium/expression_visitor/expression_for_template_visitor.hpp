#pragma once
#include "../core/toCPP.hpp"
#include "expression_visitor.hpp"

struct expression_for_template_visitor : ExpressionVisitor<expression_for_template_visitor> {
	using ExpressionVisitor<expression_for_template_visitor>::operator();

	using R = std::string;

	ExpressionVisitorDeclarations
};
