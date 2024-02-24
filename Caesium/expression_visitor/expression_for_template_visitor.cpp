#include "../core/toCPP.hpp"
//#include "expression_for_template_visitor.hpp"
#include "../utility/replace_all.hpp"

using T = expression_for_template_visitor;
using R = T::R;

R T::operator()(const NodeStructs::ConditionalExpression& expr) {
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
	std::stringstream ss;
	ss << operator()(expr.expr);
	for (const auto& e : expr.ors)
		ss << "_" << operator()(e);
	return ss.str();
}

R T::operator()(const NodeStructs::AndExpression& expr) {
	std::stringstream ss;
	ss << operator()(expr.expr);
	for (const auto& e : expr.ands)
		ss << "_" << operator()(e);
	return ss.str();
}

R T::operator()(const NodeStructs::EqualityExpression& expr) {
	std::stringstream ss;
	ss << operator()(expr.expr);
	for (const auto& [op, e] : expr.equals)
		ss << " " << symbol_variant_as_text(op) << " " << operator()(e);
	return ss.str();
}

R T::operator()(const NodeStructs::CompareExpression& expr) {
	std::stringstream ss;
	ss << operator()(expr.expr);
	for (const auto& [op, e] : expr.comparisons)
		ss << " " << symbol_variant_as_text(op) << " " << operator()(e);
	return ss.str();
}

R T::operator()(const NodeStructs::AdditiveExpression& expr) {
	std::stringstream ss;
	ss << operator()(expr.expr);
	for (const auto& [op, e] : expr.adds)
		ss << " " << symbol_variant_as_text(op) << " " << operator()(e);
	return ss.str();
}

R T::operator()(const NodeStructs::MultiplicativeExpression& expr) {
	std::stringstream ss;
	ss << operator()(expr.expr);
	for (const auto& [op, e] : expr.muls)
		ss << " " << symbol_variant_as_text(op) << " " << operator()(e);
	return ss.str();
}

R T::operator()(const NodeStructs::UnaryExpression& expr) {
	return operator()(expr.expr);
}

R T::operator()(const NodeStructs::CallExpression& expr) {
	auto args = expr.arguments.args | LIFT_TRANSFORM(std::get<NodeStructs::Expression>) | LIFT_TRANSFORM(operator());
	std::stringstream ss;
	for (const auto& arg : args)
		ss << "_" << arg;
	auto operand_repr = operator()(expr.operand);
	return std::move(operand_repr) + "_" + ss.str() + "_";
}

R T::operator()(const NodeStructs::TemplateExpression& expr) {
	throw;
}

R T::operator()(const NodeStructs::ConstructExpression& expr) {
	throw;
}

R T::operator()(const NodeStructs::BracketAccessExpression& expr) {
	throw;
}

R T::operator()(const NodeStructs::PropertyAccessAndCallExpression& expr) {
	throw;
}

R T::operator()(const NodeStructs::PropertyAccessExpression& expr) {
	throw;
}

R T::operator()(const NodeStructs::ParenArguments& expr) {
	auto args = expr.args | LIFT_TRANSFORM(std::get<NodeStructs::Expression>) | LIFT_TRANSFORM(operator());
	std::stringstream ss;
	for (const auto& arg : args)
		ss << "_" << arg;
	return "_" + ss.str() + "_";
}

R T::operator()(const NodeStructs::BraceArguments& expr) {
	auto args = expr.args | LIFT_TRANSFORM(std::get<NodeStructs::Expression>) | LIFT_TRANSFORM(operator());
	std::stringstream ss;
	for (const auto& arg : args)
		ss << "_" << arg;
	return "_" + ss.str() + "_";
}

R T::operator()(const std::string& expr) {
	return replace_all(
		expr,
		" ", "",
		"<", "_open_",
		">", "_close_",
		",", "_",
		".", "_"
	);
}

R T::operator()(const Token<INTEGER_NUMBER>& expr) {
	return operator()(expr.value);
}

R T::operator()(const Token<FLOATING_POINT_NUMBER>& expr) {
	return operator()(expr.value);
}

R T::operator()(const Token<STRING>& expr) {
	return "_" + operator()(expr.value) + "_";
}
