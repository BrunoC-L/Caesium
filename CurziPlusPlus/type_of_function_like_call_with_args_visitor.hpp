#pragma once
#include "toCPP.h"
#include "type_category_visitor.hpp"

struct type_of_function_like_call_with_args_visitor : TypeCategoryVisitor<type_of_function_like_call_with_args_visitor> {
	using TypeCategoryVisitor<type_of_function_like_call_with_args_visitor>::operator();

	variables_t& variables;
	const Named& named;
	const std::vector<NodeStructs::FunctionArgument>& args;

	using R = std::expected<std::pair<NodeStructs::ValueCategory, NodeStructs::TypeCategory>, user_error>;

	R operator()(const NodeStructs::Type& t);
	R operator()(const NodeStructs::TypeTemplateInstanceType& t);
	R operator()(const NodeStructs::AggregateType& t);
	R operator()(const NodeStructs::TypeType& t);
	R operator()(const NodeStructs::TypeTemplateType t);
	R operator()(const NodeStructs::FunctionType& t);
	R operator()(const NodeStructs::FunctionTemplateType& t);
	R operator()(const NodeStructs::UnionType& t);
};
