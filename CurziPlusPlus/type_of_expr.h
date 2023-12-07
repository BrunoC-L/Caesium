#pragma once

#include "toCPP.h"

NodeStructs::TypeVariant type_of_expr(
	std::map<std::string, std::vector<NodeStructs::TypeVariant>>& variables,
	const Named& named,
	const NodeStructs::AssignmentExpression& expr
);

NodeStructs::TypeVariant type_of_expr(
	std::map<std::string, std::vector<NodeStructs::TypeVariant>>& variables,
	const Named& named,
	const NodeStructs::ConditionalExpression& expr
);

NodeStructs::TypeVariant type_of_expr(
	std::map<std::string, std::vector<NodeStructs::TypeVariant>>& variables,
	const Named& named,
	const NodeStructs::OrExpression& expr
);

NodeStructs::TypeVariant type_of_expr(
	std::map<std::string, std::vector<NodeStructs::TypeVariant>>& variables,
	const Named& named,
	const NodeStructs::AndExpression& expr
);

NodeStructs::TypeVariant type_of_expr(
	std::map<std::string, std::vector<NodeStructs::TypeVariant>>& variables,
	const Named& named,
	const NodeStructs::EqualityExpression& expr
);

NodeStructs::TypeVariant type_of_expr(
	std::map<std::string, std::vector<NodeStructs::TypeVariant>>& variables,
	const Named& named,
	const NodeStructs::CompareExpression& expr
);

NodeStructs::TypeVariant type_of_expr(
	std::map<std::string, std::vector<NodeStructs::TypeVariant>>& variables,
	const Named& named,
	const NodeStructs::AdditiveExpression& expr
);

NodeStructs::TypeVariant type_of_expr(
	std::map<std::string, std::vector<NodeStructs::TypeVariant>>& variables,
	const Named& named,
	const NodeStructs::MultiplicativeExpression& expr
);

NodeStructs::TypeVariant type_of_expr(
	std::map<std::string, std::vector<NodeStructs::TypeVariant>>& variables,
	const Named& named,
	const NodeStructs::UnaryExpression& expr
);

NodeStructs::TypeVariant type_of_expr(
	std::map<std::string, std::vector<NodeStructs::TypeVariant>>& variables,
	const Named& named,
	const NodeStructs::PostfixExpression& expr
);

NodeStructs::TypeVariant type_of_expr(
	std::map<std::string, std::vector<NodeStructs::TypeVariant>>& variables,
	const Named& named,
	const NodeStructs::ParenArguments& expr
);

NodeStructs::TypeVariant type_of_expr(
	std::map<std::string, std::vector<NodeStructs::TypeVariant>>& variables,
	const Named& named,
	const NodeStructs::BraceArguments& expr
);

NodeStructs::TypeVariant type_of_expr(
	std::map<std::string, std::vector<NodeStructs::TypeVariant>>& variables,
	const Named& named,
	const std::string& expr
);

NodeStructs::TypeVariant type_of_expr(
	std::map<std::string, std::vector<NodeStructs::TypeVariant>>& variables,
	const Named& named,
	const Token<NUMBER>& expr
);

NodeStructs::TypeVariant type_of_expr(
	std::map<std::string, std::vector<NodeStructs::TypeVariant>>& variables,
	const Named& named,
	const NodeStructs::Expression& expr
);