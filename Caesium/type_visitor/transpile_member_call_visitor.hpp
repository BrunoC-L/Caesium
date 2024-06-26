#pragma once
#include "../core/toCPP.hpp"
#include "type_category_visitor.hpp"

struct transpile_member_call_visitor : TypeCategoryVisitor<transpile_member_call_visitor> {
	using TypeCategoryVisitor<transpile_member_call_visitor>::operator();

	transpilation_state_with_indent state;
	variables_t& variables;

	const non_type_information& operand_info;
	const std::string& property_name;
	const std::vector<NodeStructs::FunctionArgument>& arguments;

	using R = transpile_expression_information_t;

	TypeCategoryVisitorDeclarations
};

transpile_expression_information_t transpile_member_call(
	transpilation_state_with_indent state,
	variables_t& variables,
	const auto& operand_info,
	const std::string& property_name,
	const std::vector<NodeStructs::FunctionArgument>& arguments
) {
	return transpile_member_call_visitor{ {}, state, variables, operand_info, property_name, arguments }(operand_info.type);
}
