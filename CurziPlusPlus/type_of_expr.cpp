#include "toCPP.h"
#include "type_of_typename.h"
#include "type_of_expr.h"
#include <ranges>

std::expected<std::pair<NodeStructs::ValueCategory, NodeStructs::TypeCategory>, user_error> type_of_expr(
	variables_t& variables,
	const Named& named,
	const NodeStructs::AssignmentExpression& expr
) {
	throw;
}

std::expected<std::pair<NodeStructs::ValueCategory, NodeStructs::TypeCategory>, user_error> type_of_expr(
	variables_t& variables,
	const Named& named,
	const NodeStructs::ConditionalExpression& expr
) {
	throw;
}

std::expected<std::pair<NodeStructs::ValueCategory, NodeStructs::TypeCategory>, user_error> type_of_expr(
	variables_t& variables,
	const Named& named,
	const NodeStructs::OrExpression& expr
) {
	throw;
}

std::expected<std::pair<NodeStructs::ValueCategory, NodeStructs::TypeCategory>, user_error> type_of_expr(
	variables_t& variables,
	const Named& named,
	const NodeStructs::AndExpression& expr
) {
	throw;
}

std::expected<std::pair<NodeStructs::ValueCategory, NodeStructs::TypeCategory>, user_error> type_of_expr(
	variables_t& variables,
	const Named& named,
	const NodeStructs::EqualityExpression& expr
) {
	throw;
}

std::expected<std::pair<NodeStructs::ValueCategory, NodeStructs::TypeCategory>, user_error> type_of_expr(
	variables_t& variables,
	const Named& named,
	const NodeStructs::CompareExpression& expr
) {
	throw;
}

std::expected<std::pair<NodeStructs::ValueCategory, NodeStructs::TypeCategory>, user_error> type_of_expr(
	variables_t& variables,
	const Named& named,
	const NodeStructs::AdditiveExpression& expr
) {
	throw;
}

std::expected<std::pair<NodeStructs::ValueCategory, NodeStructs::TypeCategory>, user_error> type_of_expr(
	variables_t& variables,
	const Named& named,
	const NodeStructs::MultiplicativeExpression& expr
) {
	throw;
}

std::expected<std::pair<NodeStructs::ValueCategory, NodeStructs::TypeCategory>, user_error> type_of_expr(
	variables_t& variables,
	const Named& named,
	const NodeStructs::UnaryExpression& expr
) {
	throw;
}

std::expected<std::pair<NodeStructs::ValueCategory, NodeStructs::TypeCategory>, user_error> type_of_expr(
	variables_t& variables,
	const Named& named,
	const NodeStructs::PostfixExpression& expr
) {
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

std::expected<std::pair<NodeStructs::ValueCategory, NodeStructs::TypeCategory>, user_error> type_of_expr(
	variables_t& variables,
	const Named& named,
	const NodeStructs::ParenArguments& expr
) {
	throw;
}

std::expected<std::pair<NodeStructs::ValueCategory, NodeStructs::TypeCategory>, user_error> type_of_expr(
	variables_t& variables,
	const Named& named,
	const NodeStructs::BraceArguments& expr
) {
	// man this sucks
	std::vector<std::pair<NodeStructs::ValueCategory, NodeStructs::TypeCategory>> vec;
	vec.reserve(expr.args.size());
	for (const auto& e : expr.args) {
		auto t_or_e = type_of_expr(variables, named, std::get<NodeStructs::Expression>(e));
		if (!t_or_e.has_value())
			return std::unexpected{ std::move(t_or_e).error() };
		else
			vec.push_back(std::move(t_or_e).value());
	}
	return std::pair{
		NodeStructs::Reference{},
		NodeStructs::TypeCategory{ NodeStructs::TypeAggregate{ std::move(vec) } }
	};
}

std::expected<std::pair<NodeStructs::ValueCategory, NodeStructs::TypeCategory>, user_error> type_of_expr(
	variables_t& variables,
	const Named& named,
	const std::string& expr
) {
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
				NodeStructs::TypeCategory{ NodeStructs::TypeType{ *it->second } }
		};
	}
	{
		auto it = named.function_templates.find(expr);
		if (it != named.function_templates.end())
			return std::pair{
				NodeStructs::Reference{},
				NodeStructs::TypeCategory{ NodeStructs::FunctionTemplateType{ *it->second } }
		};
	}
	auto x = transpile(variables, named, expr);
	if (!x.has_value())
		return std::unexpected{ std::move(x).error() };
	else
		return std::unexpected{ user_error{ "could not find variable named " + std::move(x).value() } };
}

std::expected<std::pair<NodeStructs::ValueCategory, NodeStructs::TypeCategory>, user_error> type_of_expr(
	variables_t& variables,
	const Named& named,
	const Token<NUMBER>& expr
) {
	throw;
}

std::expected<std::pair<NodeStructs::ValueCategory, NodeStructs::TypeCategory>, user_error> type_of_expr(
	variables_t& variables,
	const Named& named,
	const Token<STRING>& expr
) {
	throw;
}

std::expected<std::pair<NodeStructs::ValueCategory, NodeStructs::TypeCategory>, user_error> type_of_expr(
	variables_t& variables,
	const Named& named,
	const NodeStructs::Expression& expr
) {
	return std::visit(
		[&](const auto& expr) {
			return type_of_expr(variables, named, expr);
		},
		expr.expression.get()
	);
}
