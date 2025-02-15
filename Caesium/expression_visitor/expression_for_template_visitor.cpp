#include "../core/toCPP.hpp"
#include "expression_for_template_visitor.hpp"
#include "../utility/replace_all.hpp"

using T = expression_for_template_visitor;
using R = T::R;

R T::operator()(const NodeStructs::ConditionalExpression& expr) {
	NOT_IMPLEMENTED;
	if (expr.ifElseExprs.has_value()) {
		return std::string("_") +
			operator()(expr.ifElseExprs.value().first) +
			operator()(expr.expr) +
			operator()(expr.ifElseExprs.value().second) +
			"_";
	}
	else
		return operator()(expr.expr);
}

R T::operator()(const NodeStructs::OrExpression& expr) {
	NOT_IMPLEMENTED;
	std::stringstream ss;
	ss << operator()(expr.expr);
	for (const auto& e : expr.ors)
		ss << "_" << operator()(e);
	return ss.str();
}

R T::operator()(const NodeStructs::AndExpression& expr) {
	NOT_IMPLEMENTED;
	std::stringstream ss;
	ss << operator()(expr.expr);
	for (const auto& e : expr.ands)
		ss << "_" << operator()(e);
	return ss.str();
}

R T::operator()(const NodeStructs::EqualityExpression& expr) {
	NOT_IMPLEMENTED;
	std::stringstream ss;
	ss << operator()(expr.expr);
	for (const auto& [op, e] : expr.equals)
		ss << " " << symbol_variant_as_text(op._value) << " " << operator()(e);
	return ss.str();
}

R T::operator()(const NodeStructs::CompareExpression& expr) {
	NOT_IMPLEMENTED;
	std::stringstream ss;
	ss << operator()(expr.expr);
	for (const auto& [op, e] : expr.comparisons)
		ss << " " << symbol_variant_as_text(op._value) << " " << operator()(e);
	return ss.str();
}

R T::operator()(const NodeStructs::AdditiveExpression& expr) {
	NOT_IMPLEMENTED;
	std::stringstream ss;
	ss << operator()(expr.expr);
	for (const auto& [op, e] : expr.adds)
		ss << " " << symbol_variant_as_text(op._value) << " " << operator()(e);
	return ss.str();
}

R T::operator()(const NodeStructs::MultiplicativeExpression& expr) {
	NOT_IMPLEMENTED;
	std::stringstream ss;
	ss << operator()(expr.expr);
	for (const auto& [op, e] : expr.muls)
		ss << " " << symbol_variant_as_text(op._value) << " " << operator()(e);
	return ss.str();
}

R T::operator()(const NodeStructs::UnaryExpression& expr) {
	NOT_IMPLEMENTED;
	return operator()(expr.expr);
}

R T::operator()(const NodeStructs::CallExpression& expr) {
	NOT_IMPLEMENTED;
	/*auto args = expr.arguments.args
		| std::views::transform([&](auto&& e) { return e.expr; })
		| std::views::transform([&](auto&& e) { return operator()(e); });
	std::stringstream ss;
	for (const auto& arg : args)
		ss << "_" << arg;
	auto operand_repr = operator()(expr.operand);
	return std::move(operand_repr) + "_" + ss.str() + "_";*/
}

R T::operator()(const NodeStructs::NamespaceExpression& expr) {
	NOT_IMPLEMENTED;
}

R T::operator()(const NodeStructs::TemplateExpression& expr) {
	NOT_IMPLEMENTED;
	/*auto args = expr.arguments.args
		| std::views::transform([&](auto&& e) { return make_expression({ copy(e.expression.get()) }); })
		| std::views::transform([&](auto&& e) { return operator()(e); });
	auto operand_repr = operator()(expr.operand);
	std::stringstream ss;
	ss << operand_repr << "__";
	for (const auto& arg : args)
		ss << "_" << arg;
	return ss.str();*/
}

R T::operator()(const NodeStructs::ConstructExpression& expr) {
	NOT_IMPLEMENTED;
}

R T::operator()(const NodeStructs::BracketAccessExpression& expr) {
	NOT_IMPLEMENTED;
}

R T::operator()(const NodeStructs::PropertyAccessAndCallExpression& expr) {
	NOT_IMPLEMENTED;
}

R T::operator()(const NodeStructs::PropertyAccessExpression& expr) {
	NOT_IMPLEMENTED;
}

R T::operator()(const NodeStructs::ParenArguments& expr) {
	NOT_IMPLEMENTED;
	/*auto args = expr.args
		| std::views::transform([&](auto&& e) { return e.expr; })
		| std::views::transform([&](auto&& e) { return operator()(e); });
	std::stringstream ss;
	for (const auto& arg : args)
		ss << "_" << arg;
	return "_" + ss.str() + "_";*/
}

R T::operator()(const NodeStructs::BraceArguments& expr) {
	NOT_IMPLEMENTED;
	/*auto args = expr.args
		| std::views::transform([&](auto&& e) { return e.expr; })
		| std::views::transform([&](auto&& e) { return operator()(e); });
	std::stringstream ss;
	for (const auto& arg : args)
		ss << "_" << arg;
	return "_" + ss.str() + "_";*/
}

R T::operator()(const std::string& expr) {
	if (expr == "Word")
		NOT_IMPLEMENTED;
	return expr;
}

R T::operator()(const Token<INTEGER_NUMBER>& expr) {
	NOT_IMPLEMENTED;
	return operator()(expr.value);
}

R T::operator()(const Token<FLOATING_POINT_NUMBER>& expr) {
	NOT_IMPLEMENTED;
	return operator()(expr.value);
}

R T::operator()(const Token<STRING>& expr) {
	NOT_IMPLEMENTED;
	// value has quotes i think
	return "_" + operator()(expr.value) + "_";
}
