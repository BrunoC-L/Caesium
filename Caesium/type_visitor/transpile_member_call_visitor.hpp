#pragma once
#include "../core/toCPP.hpp"
#include "type_category_visitor.hpp"

struct transpile_member_call_visitor : TypeCategoryVisitor<transpile_member_call_visitor> {
	using TypeCategoryVisitor<transpile_member_call_visitor>::operator();

	transpilation_state_with_indent state;

	const NodeStructs::Expression expr;
	const std::string& property_name;
	const std::vector<NodeStructs::FunctionArgument>& arguments;

	using R = transpile_t2;

	TypeCategoryVisitorDeclarations
};

transpile_t2 transpile_member_call(
	transpilation_state_with_indent state,
	const NodeStructs::Expression expr,
	const std::string& property_name,
	const std::vector<NodeStructs::FunctionArgument>& arguments,
	const auto& expr_t
) {
	return transpile_member_call_visitor{ {}, state, expr, property_name, arguments }(expr_t);
}
