#pragma once
#include "toCPP.hpp"
#include "type_category_visitor.hpp"

struct type_of_postfix_member_visitor : TypeCategoryVisitor<type_of_postfix_member_visitor> {
	using TypeCategoryVisitor<type_of_postfix_member_visitor>::operator();

	transpilation_state& state;
	const std::string& property_name;

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
