#pragma once
#include "toCPP.hpp"
#include "type_category_visitor.hpp"

struct traverse_type_visitor : TypeCategoryVisitor<traverse_type_visitor> {
	using TypeCategoryVisitor<traverse_type_visitor>::operator();

	transpilation_state_with_indent state;

	using R = std::optional<error>;

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
