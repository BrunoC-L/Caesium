#pragma once
#include "toCPP.h"

std::expected<std::pair<NodeStructs::ValueCategory, NodeStructs::TypeVariant>, user_error> type_of_expr(
	variables_t& variables,
	const Named& named,
	const NodeStructs::AssignmentExpression& expr
);

std::expected<std::pair<NodeStructs::ValueCategory, NodeStructs::TypeVariant>, user_error> type_of_expr(
	variables_t& variables,
	const Named& named,
	const NodeStructs::ConditionalExpression& expr
);

std::expected<std::pair<NodeStructs::ValueCategory, NodeStructs::TypeVariant>, user_error> type_of_expr(
	variables_t& variables,
	const Named& named,
	const NodeStructs::OrExpression& expr
);

std::expected<std::pair<NodeStructs::ValueCategory, NodeStructs::TypeVariant>, user_error> type_of_expr(
	variables_t& variables,
	const Named& named,
	const NodeStructs::AndExpression& expr
);

std::expected<std::pair<NodeStructs::ValueCategory, NodeStructs::TypeVariant>, user_error> type_of_expr(
	variables_t& variables,
	const Named& named,
	const NodeStructs::EqualityExpression& expr
);

std::expected<std::pair<NodeStructs::ValueCategory, NodeStructs::TypeVariant>, user_error> type_of_expr(
	variables_t& variables,
	const Named& named,
	const NodeStructs::CompareExpression& expr
);

std::expected<std::pair<NodeStructs::ValueCategory, NodeStructs::TypeVariant>, user_error> type_of_expr(
	variables_t& variables,
	const Named& named,
	const NodeStructs::AdditiveExpression& expr
);

std::expected<std::pair<NodeStructs::ValueCategory, NodeStructs::TypeVariant>, user_error> type_of_expr(
	variables_t& variables,
	const Named& named,
	const NodeStructs::MultiplicativeExpression& expr
);

std::expected<std::pair<NodeStructs::ValueCategory, NodeStructs::TypeVariant>, user_error> type_of_expr(
	variables_t& variables,
	const Named& named,
	const NodeStructs::UnaryExpression& expr
);

std::expected<std::pair<NodeStructs::ValueCategory, NodeStructs::TypeVariant>, user_error> type_of_expr(
	variables_t& variables,
	const Named& named,
	const NodeStructs::PostfixExpression& expr
);

std::expected<std::pair<NodeStructs::ValueCategory, NodeStructs::TypeVariant>, user_error> type_of_expr(
	variables_t& variables,
	const Named& named,
	const NodeStructs::ParenArguments& expr
);

std::expected<std::pair<NodeStructs::ValueCategory, NodeStructs::TypeVariant>, user_error> type_of_expr(
	variables_t& variables,
	const Named& named,
	const NodeStructs::BraceArguments& expr
);

std::expected<std::pair<NodeStructs::ValueCategory, NodeStructs::TypeVariant>, user_error> type_of_expr(
	variables_t& variables,
	const Named& named,
	const std::string& expr
);

std::expected<std::pair<NodeStructs::ValueCategory, NodeStructs::TypeVariant>, user_error> type_of_expr(
	variables_t& variables,
	const Named& named,
	const Token<NUMBER>& expr
);

std::expected<std::pair<NodeStructs::ValueCategory, NodeStructs::TypeVariant>, user_error> type_of_expr(
	variables_t& variables,
	const Named& named,
	const NodeStructs::Expression& expr
);