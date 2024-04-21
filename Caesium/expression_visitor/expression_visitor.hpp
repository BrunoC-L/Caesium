#pragma once
#include "../core/node_structs.hpp"

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
			t.expression.get()._value
		);
	}
};

#define ExpressionExpands_(Expand) \
	Expand(NodeStructs::ConditionalExpression)\
	Expand(NodeStructs::OrExpression)\
	Expand(NodeStructs::AndExpression)\
	Expand(NodeStructs::EqualityExpression)\
	Expand(NodeStructs::CompareExpression)\
	Expand(NodeStructs::AdditiveExpression)\
	Expand(NodeStructs::MultiplicativeExpression)\
	Expand(NodeStructs::UnaryExpression)\
	Expand(NodeStructs::CallExpression)\
	Expand(NodeStructs::NamespaceExpression)\
	Expand(NodeStructs::TemplateExpression)\
	Expand(NodeStructs::ConstructExpression)\
	Expand(NodeStructs::BracketAccessExpression)\
	Expand(NodeStructs::PropertyAccessAndCallExpression)\
	Expand(NodeStructs::PropertyAccessExpression)\
	Expand(NodeStructs::ParenArguments)\
	Expand(NodeStructs::BraceArguments)\
	Expand(std::string)\
	Expand(Token<FLOATING_POINT_NUMBER>)\
	Expand(Token<INTEGER_NUMBER>)\
	Expand(Token<STRING>)

#define expr_decl(T) R operator()(const T& expr);
#define ExpressionVisitorDeclarations ExpressionExpands_(expr_decl)

#define ExpressionExpands(Expand) Expand(NodeStructs::Expression) ExpressionExpands_(Expand)
