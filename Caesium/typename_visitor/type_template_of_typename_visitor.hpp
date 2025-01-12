#pragma once
#include "../core/toCPP.hpp"
#include "typename_visitor.hpp"

struct type_template_of_typename_visitor : TypenameVisitor<type_template_of_typename_visitor> {
	using TypenameVisitor<type_template_of_typename_visitor>::operator();

	transpilation_state_with_indent state;
	variables_t& variables;
	const std::vector<NodeStructs::WordTypenameOrExpression>& templated_with;

	using R = expected<NodeStructs::MetaType>;

	TypenameVisitorDeclarations
};

expected<NodeStructs::MetaType> type_template_of_typename(
	transpilation_state_with_indent state,
	variables_t& variables,
	const std::vector<NodeStructs::WordTypenameOrExpression>& templated_with,
	const auto& tn
) {
	return type_template_of_typename_visitor{ {}, state, variables, templated_with }(tn);
}
