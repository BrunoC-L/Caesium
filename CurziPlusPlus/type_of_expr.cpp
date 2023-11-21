#include "toCPP.h"
#include "type_of_typename.h"
#include "type_of_expr.h"
#include "methods_of_type.h"

NodeStructs::TypeOrTypeTemplateInstance type_of_expr(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::AssignmentExpression& expr
) {
	throw std::runtime_error("");
}

NodeStructs::TypeOrTypeTemplateInstance type_of_expr(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::ConditionalExpression& expr
) {
	throw std::runtime_error("");
}

NodeStructs::TypeOrTypeTemplateInstance type_of_expr(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::OrExpression& expr
) {
	throw std::runtime_error("");
}

NodeStructs::TypeOrTypeTemplateInstance type_of_expr(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::AndExpression& expr
) {
	throw std::runtime_error("");
}

NodeStructs::TypeOrTypeTemplateInstance type_of_expr(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::EqualityExpression& expr
) {
	throw std::runtime_error("");
}

NodeStructs::TypeOrTypeTemplateInstance type_of_expr(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::CompareExpression& expr
) {
	throw std::runtime_error("");
}

NodeStructs::TypeOrTypeTemplateInstance type_of_expr(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::AdditiveExpression& expr
) {
	throw std::runtime_error("");
}

NodeStructs::TypeOrTypeTemplateInstance type_of_expr(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::MultiplicativeExpression& expr
) {
	throw std::runtime_error("");
}

NodeStructs::TypeOrTypeTemplateInstance type_of_expr(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::UnaryExpression& expr
) {
	throw std::runtime_error("");
}

NodeStructs::TypeOrTypeTemplateInstance type_of_expr(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::PostfixExpression& expr
) {
	/*if (expr.postfixes.size() == 0) {
		return type_of_expr(variables, named, expr.expr);
	}
	else {
		auto next_t = type_of_expr(variables, named, expr.expr);
		for (const NodeStructs::PostfixExpression::op_types& postfix : expr.postfixes) {
			next_t = std::visit(overload(
				[&](const std::string& property_name) {
					auto methods = methods_of_type(variables, named, next_t);
					throw std::runtime_error("");
					return next_t;
				},
				[&](const NodeStructs::ParenExpression& call) {
					throw std::runtime_error("");
					return next_t;
				},
				[&](const NodeStructs::BracketArguments& access) {
					throw std::runtime_error("");
					return next_t;
				},
				[&](const NodeStructs::BraceExpression& construct) {
					throw std::runtime_error("");
					return next_t;
				},
				[&](const NodeStructs::ParenArguments& call) {
					throw std::runtime_error("");
					return next_t;
				},
				[&](const NodeStructs::BraceArguments& construct) {
					throw std::runtime_error("");
					return next_t;
				},
				[&](const Token<PLUSPLUS>& e) {
					throw std::runtime_error("");
					return next_t;
				},
				[&](const Token<MINUSMINUS>& e) {
					throw std::runtime_error("");
					return next_t;
				}
				),
				postfix
			);
		}
		return next_t;
	}*/
	throw std::runtime_error("");
}

NodeStructs::TypeOrTypeTemplateInstance type_of_expr(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::ParenExpression& expr
) {
	throw std::runtime_error("");
}

NodeStructs::TypeOrTypeTemplateInstance type_of_expr(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::BraceExpression& expr
) {
	throw std::runtime_error("");
}

NodeStructs::TypeOrTypeTemplateInstance type_of_expr(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const std::string& expr
) {
	if (variables.find(expr) == variables.end()) {
		auto err = "could not find variable named " + transpile(variables, named, expr).value();
		throw std::runtime_error(err);
	}
	return variables.at(expr).back();
}

NodeStructs::TypeOrTypeTemplateInstance type_of_expr(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const Token<NUMBER>& expr
) {
	throw std::runtime_error("");
}

NodeStructs::TypeOrTypeTemplateInstance type_of_expr(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::Expression& expr
) {
	return std::visit(
		[&](const auto& expr) -> NodeStructs::TypeOrTypeTemplateInstance {
			return type_of_expr(variables, named, expr);
		},
		expr.expression.get()
	);
}
