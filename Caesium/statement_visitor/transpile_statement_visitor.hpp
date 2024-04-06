#pragma once
#include "../core/toCPP.hpp"
#include "statement_visitor.hpp"

struct transpile_statement_visitor : StatementVisitor<transpile_statement_visitor> {
	using StatementVisitor<transpile_statement_visitor>::operator();

	transpilation_state_with_indent state;
	const NodeStructs::MetaType& expected_return_type;

	using R = transpile_t;

	StatementVisitorDeclarations
};

transpile_t transpile_statement(
	transpilation_state_with_indent state,
	const auto& statement,
	const NodeStructs::MetaType& expected_return_type
) {
	return transpile_statement_visitor{ {}, state, expected_return_type }(statement);
}