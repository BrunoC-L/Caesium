#pragma once
#include "toCPP.hpp"
#include "type_category_visitor.hpp"

struct type_of_function_like_call_with_args_visitor : TypeCategoryVisitor<type_of_function_like_call_with_args_visitor> {
	using TypeCategoryVisitor<type_of_function_like_call_with_args_visitor>::operator();

	transpilation_state_with_indent state;
	const std::vector<NodeStructs::FunctionArgument>& args;

	using R = expected<std::pair<NodeStructs::ParameterCategory, NodeStructs::UniversalType>>;

	R operator()(const NodeStructs::Type& t);
	R operator()(const NodeStructs::TypeTemplateInstanceType& t);
	R operator()(const NodeStructs::AggregateType& t);
	R operator()(const NodeStructs::TypeType& t);
	R operator()(const NodeStructs::TypeTemplateType t);
	R operator()(const NodeStructs::FunctionType& t);
	R operator()(const NodeStructs::FunctionTemplateType& t);
	R operator()(const NodeStructs::FunctionTemplateInstanceType& t);
	R operator()(const NodeStructs::UnionType& t);
	R operator()(const std::string& t);
	R operator()(const double& t);
	R operator()(const int& t);
};
