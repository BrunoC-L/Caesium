#pragma once
#include "../core/toCPP.hpp"
#include "expression_visitor.hpp"

struct transpile_call_expression_with_args_visitor : ExpressionVisitor<transpile_call_expression_with_args_visitor> {
	using ExpressionVisitor<transpile_call_expression_with_args_visitor>::operator();

	transpilation_state_with_indent state;

	const std::vector<NodeStructs::FunctionArgument>& arguments;

	using R = transpile_t;

	//R operator()(const NodeStructs::AssignmentExpression& expr);
	R operator()(const NodeStructs::ConditionalExpression& expr);
	R operator()(const NodeStructs::OrExpression& expr);
	R operator()(const NodeStructs::AndExpression& expr);
	R operator()(const NodeStructs::EqualityExpression& expr);
	R operator()(const NodeStructs::CompareExpression& expr);
	R operator()(const NodeStructs::AdditiveExpression& expr);
	R operator()(const NodeStructs::MultiplicativeExpression& expr);
	R operator()(const NodeStructs::UnaryExpression& expr);
	R operator()(const NodeStructs::CallExpression& expr);
	R operator()(const NodeStructs::TemplateExpression& expr);
	R operator()(const NodeStructs::ConstructExpression& expr);
	R operator()(const NodeStructs::BracketAccessExpression& expr);
	R operator()(const NodeStructs::PropertyAccessExpression& expr);
	R operator()(const NodeStructs::ParenArguments& expr);
	R operator()(const NodeStructs::BraceArguments& expr);
	R operator()(const std::string& expr);
	R operator()(const Token<FLOATING_POINT_NUMBER>& expr);
	R operator()(const Token<INTEGER_NUMBER>& expr);
	R operator()(const Token<STRING>& expr);
};
