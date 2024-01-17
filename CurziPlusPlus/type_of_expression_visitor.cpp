#include "type_of_expression_visitor.hpp"
#include "transpile_expression_visitor.hpp"

using T = type_of_expression_visitor;
using R = T::R;

//R T::operator()(const NodeStructs::AssignmentExpression& expr) {
//	throw;
//}

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
	return R::value_type{ NodeStructs::Value{}, std::reference_wrapper{ *state.state.named.types.at("Bool").at(0) } };
}

R T::operator()(const NodeStructs::AdditiveExpression& expr) {
	auto type_of_first_term = operator()(expr.expr);
	return_if_error(type_of_first_term);
	return std::pair{
		NodeStructs::Value{}, type_of_first_term.value().second
	};
}

R T::operator()(const NodeStructs::MultiplicativeExpression& expr) {
	throw;
}

R T::operator()(const NodeStructs::UnaryExpression& expr) {
	throw;
}

R T::operator()(const NodeStructs::PostfixExpression& expr) {
	throw;
	/*if (expr.postfixes.size() == 0) {
		return type_of_expr(variables, named, expr.expr);
	}
	else {
		auto next_t = type_of_expr(variables, named, expr.expr);
		for (const NodeStructs::PostfixExpression::op_types& postfix : expr.postfixes) {
			next_t = std::visit(overload(
				[&](const std::string& property_name) {
					auto methods = methods_of_type(variables, named, next_t);
					throw;
					return next_t;
				},
				[&](const NodeStructs::ParenExpression& call) {
					throw;
					return next_t;
				},
				[&](const NodeStructs::BracketArguments& access) {
					throw;
					return next_t;
				},
				[&](const NodeStructs::BraceExpression& construct) {
					throw;
					return next_t;
				},
				[&](const NodeStructs::ParenArguments& call) {
					throw;
					return next_t;
				},
				[&](const NodeStructs::BraceArguments& construct) {
					throw;
					return next_t;
				},
				[&](const Token<PLUSPLUS>& e) {
					throw;
					return next_t;
				},
				[&](const Token<MINUSMINUS>& e) {
					throw;
					return next_t;
				}
				),
				postfix
			);
		}
		return next_t;
	}*/
}

R T::operator()(const NodeStructs::ParenExpression& expr) {
	throw;
}

R T::operator()(const NodeStructs::ParenArguments& expr) {
	throw;
}

R T::operator()(const NodeStructs::BraceArguments& expr) {
	// man this sucks
	std::vector<std::pair<NodeStructs::ArgumentCategory, NodeStructs::UniversalType>> vec;
	vec.reserve(expr.args.size());
	for (const auto& [arg_cat, arg] : expr.args) {
		auto type_of_expression = operator()(arg);
		return_if_error(type_of_expression);
		if (!arg_cat.has_value())
			throw;
		vec.push_back(std::pair{ arg_cat.value(), std::move(type_of_expression).value().second });
	}
	return std::pair{
		NodeStructs::Reference{},
		NodeStructs::UniversalType{ NodeStructs::AggregateType{ std::move(vec) } }
	};
}

R T::operator()(const std::string& expr) {
	if (auto it = state.state.variables.find(expr); it != state.state.variables.end())
		return it->second.back();

	if (auto it = state.state.named.types.find(expr); it != state.state.named.types.end())
		return std::pair{
			NodeStructs::Reference{},
			NodeStructs::UniversalType{ NodeStructs::TypeType{ *it->second.back() } }
	};

	if (auto it = state.state.named.function_templates.find(expr); it != state.state.named.function_templates.end())
		return std::pair{
			NodeStructs::Reference{},
			NodeStructs::UniversalType{ NodeStructs::FunctionTemplateType{ *it->second.back() } }
		};

	if (auto it = state.state.named.functions.find(expr); it != state.state.named.functions.end())
		return std::pair{
			NodeStructs::Reference{},
			NodeStructs::UniversalType{ NodeStructs::FunctionType{ *it->second.back() } }
	};

	auto x = transpile_expression_visitor{ {}, state }(expr);
	return_if_error(x);
	return error{ "user error", "could not find variable named " + std::move(x).value() };
}

R T::operator()(const Token<INTEGER_NUMBER>& expr) {
	return std::pair{
		NodeStructs::Reference{},
		NodeStructs::UniversalType{ stoi(expr.value) }
	};
}

R T::operator()(const Token<FLOATING_POINT_NUMBER>& expr) {
	return std::pair{
		NodeStructs::Reference{},
		NodeStructs::UniversalType{ stod(expr.value) }
	};
}

R T::operator()(const Token<STRING>& expr) {
	throw;
}
