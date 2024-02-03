#pragma once
#include "toCPP.hpp"
#include "type_category_visitor.hpp"

struct type_of_postfix_member_visitor : TypeCategoryVisitor<type_of_postfix_member_visitor> {
	using TypeCategoryVisitor<type_of_postfix_member_visitor>::operator();

	transpilation_state_with_indent state;
	const std::string& property_name;

	using R = expected<std::pair<NodeStructs::ParameterCategory, NodeStructs::UniversalType>>;

	R operator()(const NodeStructs::Type& t);
	R operator()(const NodeStructs::AggregateType& t);
	R operator()(const NodeStructs::TypeType& t);
	R operator()(const NodeStructs::FunctionType& t);
	R operator()(const NodeStructs::UnionType& t);
	R operator()(const NodeStructs::VectorType& t);
	R operator()(const NodeStructs::SetType& t);
	R operator()(const NodeStructs::MapType& t);
	R operator()(const NodeStructs::Template& t);
	R operator()(const std::string& t);
	R operator()(const double& t);
	R operator()(const int& t);
	R operator()(const bool& t);
};
