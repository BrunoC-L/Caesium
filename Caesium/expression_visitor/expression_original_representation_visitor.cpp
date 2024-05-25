#include "../core/toCPP.hpp"
#include "expression_original_representation_visitor.hpp"

using T = expression_original_representation_visitor;
using R = T::R;

inline std::string arg_cat_to_string(const NodeStructs::ArgumentCategory& arg_cat) {
	return std::visit(overload(
			[](const NodeStructs::Reference&) {
				return "ref ";
			},
			[](const NodeStructs::MutableReference&) {
				return "ref! ";
			},
			[](const NodeStructs::Move&) {
				return "move ";
			}
		),
		arg_cat._value
	);
}

inline std::string arg_cat_opt_to_string(const std::optional<NodeStructs::ArgumentCategory>& opt_arg_cat) {
	if (opt_arg_cat.has_value())
		return arg_cat_to_string(opt_arg_cat.value());
	else
		return "";
}

R T::operator()(const NodeStructs::ConditionalExpression& expr) {
	throw;
}

R T::operator()(const NodeStructs::OrExpression& expr) {
	throw;
}

R T::operator()(const NodeStructs::AndExpression& expr) {
	throw;
}

R T::operator()(const NodeStructs::EqualityExpression& expr) {
	throw;
}

R T::operator()(const NodeStructs::CompareExpression& expr) {
	throw;
}

R T::operator()(const NodeStructs::AdditiveExpression& expr) {
	throw;
}

R T::operator()(const NodeStructs::MultiplicativeExpression& expr) {
	throw;
}

R T::operator()(const NodeStructs::UnaryExpression& expr) {
	throw;
}

R T::operator()(const NodeStructs::CallExpression& expr) {
	throw;
}

R T::operator()(const NodeStructs::NamespaceExpression& expr) {
	throw;
}

R T::operator()(const NodeStructs::TemplateExpression& expr) {
	throw;
}

R T::operator()(const NodeStructs::ConstructExpression& expr) {
	std::stringstream ss;
	ss << typename_original_representation(expr.operand) << "{";
	for (const auto& arg : expr.arguments.args)
		ss << arg_cat_opt_to_string(arg.category) << operator()(arg.expr) << ", ";
	ss << "}";
	return ss.str();
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
	throw;
}

R T::operator()(const NodeStructs::BraceArguments& expr) {
	throw;
}

R T::operator()(const std::string& expr) {
	return expr;
}

R T::operator()(const Token<FLOATING_POINT_NUMBER>& expr) {
	return expr.value;
}

R T::operator()(const Token<INTEGER_NUMBER>& expr) {
	return expr.value;
}

R T::operator()(const Token<STRING>& expr) {
	return "'" + expr.value + "'";
}
