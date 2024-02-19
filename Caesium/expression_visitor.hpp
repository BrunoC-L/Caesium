#pragma once
#include "node_structs.hpp"

template <typename T>
concept ExpressionVisitorConcept = requires(T && t, const NodeStructs::Expression & v) {
	t(v);
	std::visit(t, v);
};

template <typename T>
struct ExpressionVisitor {
	template <typename Self>
	auto operator()(this Self&& self, const NodeStructs::Expression& t) {
		return std::visit(
			[&](const auto& t) {
				return self(t);
			},
			t.expression.get()
		);
	}
	/*
	//R operator()(const NodeStructs::AssignmentExpression& expr);
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
	R operator()(const Token<FLOATING_POINT_NUMBER>& expr);
	R operator()(const Token<INTEGER_NUMBER>& expr);
	*/
};
