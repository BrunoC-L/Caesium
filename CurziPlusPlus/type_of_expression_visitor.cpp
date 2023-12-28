#include "type_of_expression_visitor.hpp"
#include "transpile_expression_visitor.hpp"

using T = type_of_expression_visitor;
using R = T::R;

R T::operator()(const NodeStructs::AssignmentExpression& expr) {
	throw;
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

R T::operator()(const NodeStructs::ParenArguments& expr) {
	throw;
}

R T::operator()(const NodeStructs::BraceArguments& expr) {
	// man this sucks
	std::vector<std::pair<NodeStructs::ValueCategory, NodeStructs::TypeCategory>> vec;
	vec.reserve(expr.args.size());
	for (const auto& e : expr.args) {
		auto t_or_e = operator()(std::get<NodeStructs::Expression>(e));
		if (!t_or_e.has_value())
			return std::unexpected{ std::move(t_or_e).error() };
		else
			vec.push_back(std::move(t_or_e).value());
	}
	return std::pair{
		NodeStructs::Reference{},
		NodeStructs::TypeCategory{ NodeStructs::AggregateType{ std::move(vec) } }
	};
}

R T::operator()(const std::string& expr) {
	{
		auto it = variables.find(expr);
		if (it != variables.end())
			return it->second.back();
	}
	{
		auto it = named.types.find(expr);
		if (it != named.types.end())
			return std::pair{
				NodeStructs::Reference{},
				NodeStructs::TypeCategory{ NodeStructs::TypeType{ it->second.back() } }
		};
	}
	{
		auto it = named.function_templates.find(expr);
		if (it != named.function_templates.end())
			return std::pair{
				NodeStructs::Reference{},
				NodeStructs::TypeCategory{ NodeStructs::FunctionTemplateType{ it->second.back() } }
		};
	}
	{
		auto it = named.functions.find(expr);
		if (it != named.functions.end())
			return std::pair{
				NodeStructs::Reference{},
				NodeStructs::TypeCategory{ NodeStructs::FunctionType{ it->second.back() } }
		};
	}
	auto x = transpile_expression_visitor{ {}, variables, named }(expr);
	if (!x.has_value())
		return std::unexpected{ std::move(x).error() };
	else
		return std::unexpected{ user_error{ "could not find variable named " + std::move(x).value() } };
}

R T::operator()(const Token<NUMBER>& expr) {
	throw;
}

R T::operator()(const Token<STRING>& expr) {
	throw;
}
