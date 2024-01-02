#include "transpile_expression_visitor.hpp"
#include "type_of_expression_visitor.hpp"
#include "type_of_postfix_member_visitor.hpp"
#include "type_of_function_like_call_with_args_visitor.hpp"

using T = transpile_expression_visitor;
using R = T::R;

//R T::operator()(const NodeStructs::AssignmentExpression& expr) {
//	std::stringstream ss;
//	ss << operator()(expr.expr).value();
//	for (const auto& e : expr.assignments)
//		ss << " " << symbol_variant_as_text(e.first) << " " << operator()(e.second).value();
//	return ss.str();
//}

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
	auto base_expr = operator()(expr.expr);
	if (!base_expr.has_value())
		return std::unexpected{ std::move(base_expr).error() };
	ss << base_expr.value();
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
	auto t_or_e = type_of_expression_visitor{ {}, state }(expr.expr);
	if (!t_or_e.has_value())
		return std::unexpected{ std::move(t_or_e).error() };

	auto v = operator()(expr.expr);
	if (!v.has_value())
		return std::unexpected{ std::move(v).error() };

	type_and_representation it{
		std::move(t_or_e).value().second, // todo maybe type and repr has to hold value category...?
		std::move(v).value()
	};

	for (const auto& op : expr.postfixes) {
		transpile_type_repr next = std::visit(
			overload(overload_default_error,
				[&](const std::string& property_name) -> transpile_type_repr {
					auto t = type_of_postfix_member_visitor{ {}, state, property_name }(it.type);
					if (!t.has_value())
						return std::unexpected{ std::move(t).error() };
					return type_and_representation{
						std::move(t).value().second,
						it.representation + "." + property_name
					};
				},
				[&](const NodeStructs::ParenArguments& e) -> transpile_type_repr {
					auto t = type_of_function_like_call_with_args_visitor{ {}, state, e.args }(it.type);
					if (!t.has_value())
						return std::unexpected{ std::move(t).error() };
					auto v = transpile_args(state, e.args);
					if (!v.has_value())
						return std::unexpected{ std::move(v).error() };
					return type_and_representation{
						std::move(t).value().second,
						it.representation + "(" + std::move(v).value() + ")"
					};
				},
				[&](const NodeStructs::BracketArguments& e) -> transpile_type_repr {
					throw;
					//return "[" + transpile_args(state, e.args).value() + "]";
				},
				[&](const NodeStructs::BraceArguments& e) -> transpile_type_repr {
					throw;
					//return "{" + transpile_args(state, e.args).value() + "}";
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
		if (!next.has_value())
			return std::unexpected{ next.error() };
		it = std::move(next).value();
	}
	return it.representation;
}

R T::operator()(const NodeStructs::ParenExpression& expr) {
	std::stringstream ss;
	for (const auto& e : expr.args) {
		auto t = operator()(e);
		if (!t.has_value())
			return std::unexpected{ t.error() };
		ss << std::move(t).value();
	}
	return "(" + ss.str() + ")";
}

R T::operator()(const NodeStructs::ParenArguments& expr) {
	throw;
}

R T::operator()(const NodeStructs::BraceArguments& expr) {
	return "{" + transpile_args(state, expr.args).value() + "}";
}

R T::operator()(const std::string& expr) {
	if (state.state.variables.contains(expr))
		return expr;
	if (auto it = state.state.named.functions.find(expr); it != state.state.named.functions.end()) {
		const auto& e = *it->second;
		if (!state.state.traversed_functions.contains(e)) {
			state.state.traversed_functions.insert(e);
			auto t = transpile(state.unindented(), e);
			if (!t.has_value())
				return std::unexpected{ t.error() };
			state.state.transpile_in_reverse_order.push_back(std::move(t).value());
		}
		return expr;
	}
	if (auto it = state.state.named.types.find(expr); it != state.state.named.types.end()) {
		const auto& e = *it->second;
		if (!state.state.traversed_types.contains(e)) {
			state.state.traversed_types.insert(e);
			auto t = transpile(state, e);
			if (!t.has_value())
				return std::unexpected{ t.error() };
			state.state.transpile_in_reverse_order.push_back(std::move(t).value());
		}
		return expr;
	}
	return std::unexpected{ user_error{ "Undeclared variable `" + expr + "`" } };
}

R T::operator()(const Token<NUMBER>& expr) {
	return expr.value;
}

R T::operator()(const Token<STRING>& expr) {
	return expr.value;
}
