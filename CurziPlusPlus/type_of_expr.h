#pragma once

#include "toCPP.h"

NodeStructs::TypeOrTypeTemplateInstance type_of_expr(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::AssignmentExpression& expr
);

NodeStructs::TypeOrTypeTemplateInstance type_of_expr(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::ConditionalExpression& expr
);

NodeStructs::TypeOrTypeTemplateInstance type_of_expr(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::OrExpression& expr
);

NodeStructs::TypeOrTypeTemplateInstance type_of_expr(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::AndExpression& expr
);

NodeStructs::TypeOrTypeTemplateInstance type_of_expr(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::EqualityExpression& expr
);

NodeStructs::TypeOrTypeTemplateInstance type_of_expr(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::CompareExpression& expr
);

NodeStructs::TypeOrTypeTemplateInstance type_of_expr(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::AdditiveExpression& expr
);

NodeStructs::TypeOrTypeTemplateInstance type_of_expr(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::MultiplicativeExpression& expr
);

NodeStructs::TypeOrTypeTemplateInstance type_of_expr(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::UnaryExpression& expr
);

NodeStructs::TypeOrTypeTemplateInstance type_of_expr(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::PostfixExpression& expr
);

NodeStructs::TypeOrTypeTemplateInstance type_of_expr(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::ParenExpression& expr
);

NodeStructs::TypeOrTypeTemplateInstance type_of_expr(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::BraceExpression& expr
);

NodeStructs::TypeOrTypeTemplateInstance type_of_expr(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const std::string& expr
);

NodeStructs::TypeOrTypeTemplateInstance type_of_expr(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const Token<NUMBER>& expr
);

NodeStructs::TypeOrTypeTemplateInstance type_of_expr(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::Expression& expr
);