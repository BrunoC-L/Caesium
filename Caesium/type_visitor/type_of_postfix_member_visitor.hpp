#pragma once
#include "../core/toCPP.hpp"
#include "type_category_visitor.hpp"

struct type_of_postfix_member_visitor : TypeCategoryVisitor<type_of_postfix_member_visitor> {
	using TypeCategoryVisitor<type_of_postfix_member_visitor>::operator();

	transpilation_state_with_indent state;
	const std::string& property_name;

	using R = expected<std::pair<NodeStructs::ParameterCategory, NodeStructs::MetaType>>;

	TypeCategoryVisitorDeclarations
};

expected<std::pair<NodeStructs::ParameterCategory, NodeStructs::MetaType>> type_of_postfix_member(
	transpilation_state_with_indent state,
	const std::string& property_name,
	const auto& type
) {
	return type_of_postfix_member_visitor{ {}, state, property_name }(type);
}
