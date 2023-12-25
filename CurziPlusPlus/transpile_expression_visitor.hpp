#pragma once
#include "toCPP.h"
#include "expression_visitor.hpp"

struct transpile_expression_visitor : ExpressionVisitor<transpile_expression_visitor> {
	using ExpressionVisitor<transpile_expression_visitor>::operator();

	variables_t& variables;
	const Named& named;

	using R = transpile_t;

	R operator()(const NodeStructs::AssignmentExpression& expr);
	R operator()(const NodeStructs::ConditionalExpression& expr);
	R operator()(const NodeStructs::OrExpression& expr);
	R operator()(const NodeStructs::AndExpression& expr);
	R operator()(const NodeStructs::EqualityExpression& expr);
	R operator()(const NodeStructs::CompareExpression& expr);
	R operator()(const NodeStructs::AdditiveExpression& expr);
	R operator()(const NodeStructs::MultiplicativeExpression& expr);
	R operator()(const NodeStructs::UnaryExpression& expr);
	R operator()(const NodeStructs::PostfixExpression& expr);
	R operator()(const NodeStructs::ParenArguments& expr);
	R operator()(const NodeStructs::BraceArguments& expr);
	R operator()(const std::string& expr);
	R operator()(const Token<NUMBER>& expr);
	R operator()(const Token<STRING>& expr);
};