#include "transpile_expression_visitor.hpp"
#include "type_of_expression_visitor.hpp"
#include "type_of_postfix_member_visitor.hpp"
#include "type_of_function_like_call_with_args_visitor.hpp"

using T = transpile_expression_visitor;
using R = T::R;

R T::operator()(const NodeStructs::AssignmentExpression& expr) {
	std::stringstream ss;
	ss << operator()(expr.expr).value();
	for (const auto& e : expr.assignments)
		ss << " " << symbol_variant_as_text(e.first) << " " << operator()(e.second).value();
	return ss.str();
}

R T::operator()(const NodeStructs::ConditionalExpression& expr) {
	if (expr.ifElseExprs.has_value()) {
		// x if first else second
		//                  ([&] () { if (first) return x; else return second; }());
		return std::string("([&] () { if (") +
			operator()(expr.ifElseExprs.value().first).value() +
			") return " +
			operator()(expr.expr).value() +
			"; else return " +
			operator()(expr.ifElseExprs.value().second).value() +
			"; }())";
	}
	else
		return operator()(expr.expr);
}

R T::operator()(const NodeStructs::OrExpression& expr) {
	std::stringstream ss;
	ss << operator()(expr.expr).value();
	for (const auto& e : expr.ors)
		ss << " || " << operator()(e).value();
	return ss.str();
}

R T::operator()(const NodeStructs::AndExpression& expr) {
	std::stringstream ss;
	ss << operator()(expr.expr).value();
	for (const auto& e : expr.ands)
		ss << " && " << operator()(e).value();
	return ss.str();
}

R T::operator()(const NodeStructs::EqualityExpression& expr) {
	std::stringstream ss;
	ss << operator()(expr.expr).value();
	for (const auto& [op, e] : expr.equals)
		ss << " " << symbol_variant_as_text(op) << " " << operator()(e).value();
	return ss.str();
}

R T::operator()(const NodeStructs::CompareExpression& expr) {
	std::stringstream ss;
	ss << operator()(expr.expr).value();
	for (const auto& [op, e] : expr.comparisons)
		ss << " " << symbol_variant_as_text(op) << " " << operator()(e).value();
	return ss.str();
}

R T::operator()(const NodeStructs::AdditiveExpression& expr) {
	std::stringstream ss;
	ss << operator()(expr.expr).value();
	for (const auto& [op, e] : expr.adds)
		ss << " " << symbol_variant_as_text(op) << " " << operator()(e).value();
	return ss.str();
}

R T::operator()(const NodeStructs::MultiplicativeExpression& expr) {
	std::stringstream ss;
	ss << operator()(expr.expr).value();
	for (const auto& [op, e] : expr.muls)
		ss << " " << symbol_variant_as_text(op) << " " << operator()(e).value();
	std::string x = ss.str();
	return ss.str();
}

R T::operator()(const NodeStructs::UnaryExpression& expr) {
	std::stringstream ss;
	for (const auto& op : expr.unary_operators)
		ss << std::visit([&](const auto& token_expr) { return symbol_as_text(token_expr); }, op);
	ss << operator()(expr.expr).value();
	return ss.str();
}

R T::operator()(const NodeStructs::PostfixExpression& expr) {
	auto t_or_e = type_of_expression_visitor{ {}, variables, named }(expr.expr);
	if (!t_or_e.has_value())
		return std::unexpected{ std::move(t_or_e).error() };
	type_and_representation it{
		std::move(t_or_e).value().second, // todo maybe type and repr has to hold value category...?
		operator()(expr.expr).value()
	};

	for (const auto& op : expr.postfixes) {
		transpile_type_repr next = std::visit(
			overload(overload_default_error,
				[&](const std::string& property_name) -> transpile_type_repr {
					return type_of_postfix_member_visitor{ {}, variables, named, property_name }(it.type).transform(
						[&](auto t) {
							return type_and_representation{
								t.second,
								it.representation + "." + property_name
							};
						}
					);
				},
				[&](const NodeStructs::ParenArguments& e) -> transpile_type_repr {
					return type_of_function_like_call_with_args_visitor{ {}, variables, named, e.args }(it.type).transform(
						[&](std::pair<NodeStructs::ValueCategory, NodeStructs::TypeCategory>&& t) {
							return type_and_representation{
								std::move(t.second),
								it.representation + "(" + transpile_args(variables, named, e.args).value() + ")"
							};
						}
					);
				},
				[&](const NodeStructs::BracketArguments& e) -> transpile_type_repr {
					throw;
					//return "[" + transpile_args(variables, named, e.args).value() + "]";
				},
				[&](const NodeStructs::BraceArguments& e) -> transpile_type_repr {
					throw;
					//return "{" + transpile_args(variables, named, e.args).value() + "}";
				},
				[&](const Token<PLUSPLUS>& op) -> transpile_type_repr {
					throw;
					//return symbol_as_text(op);
				},
				[&](const Token<MINUSMINUS>& op) -> transpile_type_repr {
					throw;
					//return symbol_as_text(op);
				}
			),
			op
		);
		if (next.has_value())
			it = next.value();
		else
			return std::unexpected{ next.error() };
	}
	return it.representation;
}

R T::operator()(const NodeStructs::ParenArguments& expr) {
	throw;
}

R T::operator()(const NodeStructs::BraceArguments& expr) {
	return "{" + transpile_args(variables, named, expr.args).value() + "}";
}

R T::operator()(const std::string& expr) {
	if (variables.contains(expr))
		return expr;
	if (named.functions.contains(expr))
		return expr;
	if (named.function_templates.contains(expr))
		return expr;
	return std::unexpected{ user_error{ "Undeclared variable `" + expr + "`" } };
}

R T::operator()(const Token<NUMBER>& expr) {
	return expr.value;
}

R T::operator()(const Token<STRING>& expr) {
	return expr.value;
}
