#include "toCPP.h"
#include "type_of_typename.h"
#include "type_of_expr.h"

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
	if (expr.postfixes.size() == 0) {
		return type_of_expr(variables, named, expr.expr);
	}
	else {
		auto first_t = type_of_expr(variables, named, expr.expr);
		for (const NodeStructs::PostfixExpression::op_types& postfix : expr.postfixes) {
			std::visit(overload(
				[&](const std::string& property) {
					first_t = first_t;
				},
				[&](const NodeStructs::ParenExpression& call) {
					first_t = first_t;
				},
				[&](const NodeStructs::BracketArguments& access) {
				},
				[&](const NodeStructs::BraceExpression& construct) {
				},
				[&](const Token<PLUSPLUS>& e) {
				},
				[&](const Token<MINUSMINUS>& e) {
				}
				),
				postfix
			);
		}
		return first_t;
	}
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
		*expr.expression.get()
	);
}
