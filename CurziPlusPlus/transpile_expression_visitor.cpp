#include "transpile_expression_visitor.hpp"
#include "type_of_expression_visitor.hpp"
#include "type_of_postfix_member_visitor.hpp"
#include "type_of_function_like_call_with_args_visitor.hpp"
#include "type_of_template_instantiation_with_args_visitor.hpp"
#include "cursor_vector_view.hpp"
#include "traverse_type_visitor.hpp"
#include "transpile_typename_visitor.hpp";
#include <algorithm>

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
	auto base = operator()(expr.expr);
	return_if_error(base);
	ss << base.value();
	for (const auto& e : expr.ors) {
		auto repr = operator()(e);
		return_if_error(repr);
		ss << " || " << repr.value();
	}
	return ss.str();
}

R T::operator()(const NodeStructs::AndExpression& expr) {
	std::stringstream ss;
	auto base = operator()(expr.expr);
	return_if_error(base);
	ss << base.value();
	for (const auto& e : expr.ands) {
		auto repr = operator()(e);
		return_if_error(repr);
		ss << " && " << repr.value();
	}
	return ss.str();
}

R T::operator()(const NodeStructs::EqualityExpression& expr) {
	std::stringstream ss;
	auto base = operator()(expr.expr);
	return_if_error(base);
	ss << base.value();
	for (const auto& [op, e] : expr.equals) {
		auto repr = operator()(e);
		return_if_error(repr);
		ss << " " << symbol_variant_as_text(op) << " " << repr.value();
	}
	return ss.str();
}

R T::operator()(const NodeStructs::CompareExpression& expr) {
	std::stringstream ss;
	auto base = operator()(expr.expr);
	return_if_error(base);
	ss << base.value();
	for (const auto& [op, e] : expr.comparisons) {
		auto repr = operator()(e);
		return_if_error(repr);
		ss << " " << symbol_variant_as_text(op) << " " << repr.value();
	}
	return ss.str();
}

R T::operator()(const NodeStructs::AdditiveExpression& expr) {
	std::stringstream ss;
	auto base = operator()(expr.expr);
	return_if_error(base);
	ss << base.value();
	for (const auto& [op, e] : expr.adds) {
		auto repr = operator()(e);
		return_if_error(repr);
		ss << " " << symbol_variant_as_text(op) << " " << repr.value();
	}
	return ss.str();
}

R T::operator()(const NodeStructs::MultiplicativeExpression& expr) {
	std::stringstream ss;
	auto base = operator()(expr.expr);
	return_if_error(base);
	ss << base.value();
	for (const auto& [op, e] : expr.muls) {
		auto repr = operator()(e);
		return_if_error(repr);
		ss << " " << symbol_variant_as_text(op) << " " << repr.value();
	}
	std::string x = ss.str();
	return ss.str();
}

R T::operator()(const NodeStructs::UnaryExpression& expr) {
	std::stringstream ss;
	for (const auto& op : expr.unary_operators)
		ss << std::visit([&](const auto& token_expr) { return symbol_as_text(token_expr); }, op);
	auto repr = operator()(expr.expr);
	return_if_error(repr);
	ss << repr.value();
	return ss.str();
}


std::string replace_all(std::string str, const std::string& from, const std::string& to, auto&&... args) {
	if constexpr (sizeof...(args) > 0)
		str = replace_all(std::move(str), args...);
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
	}
	return str;
}

R T::operator()(const NodeStructs::PostfixExpression& expr) {
	auto t_or_e = type_of_expression_visitor{ {}, state }(expr.expr);
	return_if_error(t_or_e);

	auto v = operator()(expr.expr);
	return_if_error(v);

	type_and_representation it{
		std::move(t_or_e).value().second, // todo maybe type and repr has to hold value category...?
		std::move(v).value()
	};

	for (const auto& op : expr.postfixes) {
		transpile_type_repr next = std::visit(
			overload(overload_default_error,
				[&](const std::string& property_name) -> transpile_type_repr {
					auto t = type_of_postfix_member_visitor{ {}, state, property_name }(it.type);
					return_if_error(t);
					return type_and_representation{
						std::move(t).value().second,
						it.representation + "." + property_name
					};
				},
				[&](const NodeStructs::ParenArguments& e) -> transpile_type_repr {
					auto t = type_of_function_like_call_with_args_visitor{ {}, state, e.args }(it.type);
					return_if_error(t);
					auto v = transpile_args(state, e.args);
					return_if_error(v);

					/*if (std::holds_alternative<NodeStructs::FunctionTemplateType>(it.type.value)) {
						throw;
					}*/

					/*if (std::holds_alternative<NodeStructs::FunctionTemplateInstanceType>(it.type.value)) {
						throw;
					}*/

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
				[&](const NodeStructs::TemplateArguments& e) -> transpile_type_repr {
					auto t = type_of_template_instantiation_with_args_visitor{ {}, state, e.args }(it.type);
					return_if_error(t);
					auto v = transpile_expressions(state, e.args);
					return_if_error(v);

					std::string x = replace_all(
						std::move(v).value(),
						"<", "_open_",
						">", "_close_",
						"::", "_ns_",
						", ", "_"
					);

					return type_and_representation{
						std::move(t).value(),
						it.representation + "_open_" + std::move(x) + "_close_"
					};
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
		return_if_error(next);
		it = std::move(next).value();
	}
	return it.representation;
}

R T::operator()(const NodeStructs::ParenExpression& expr) {
	std::stringstream ss;
	for (const auto& e : expr.args) {
		auto t = operator()(e);
		return_if_error(t);
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
		const auto& e = *it->second.back();
		if (!state.state.traversed_functions.contains(e)) {
			state.state.traversed_functions.insert(e);
			auto t = transpile(state.unindented(), e);
			return_if_error(t);
			state.state.transpile_in_reverse_order.push_back(std::move(t).value());
		}
		return expr;
	}
	if (auto it = state.state.named.types.find(expr); it != state.state.named.types.end()) {
		const auto& e = *it->second.back();
		if (auto t = traverse_type_visitor{ {}, state }(e); t.has_value())
			return t.value();
		return expr;
	}
	if (auto it = state.state.named.type_aliases.find(expr); it != state.state.named.type_aliases.end()) {
		if (std::optional<error> t = traverse_type_visitor{ {}, state }(it->second); t.has_value())
			return t.value();
		return transpile_typename_visitor{ {}, state }(state.state.named.type_aliases_typenames.at(expr));
	}
	if (auto it = state.state.named.templates.find(expr); it != state.state.named.templates.end()) {
		return expr;
	}
	return error{ "user error", "Undeclared variable `" + expr + "`" };
}

R T::operator()(const Token<INTEGER_NUMBER>& expr) {
	return expr.value;
}

R T::operator()(const Token<FLOATING_POINT_NUMBER>& expr) {
	return expr.value;
}

R T::operator()(const Token<STRING>& expr) {
	return expr.value;
}
