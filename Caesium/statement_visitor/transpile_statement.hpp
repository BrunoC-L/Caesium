#include "../core/toCPP.hpp"

transpile_t transpile_statement(
	transpilation_state_with_indent state,
	variables_t& variables,
	const NodeStructs::MetaType& expected_return_type,
	const NodeStructs::Statement<function_context>& statement
);

transpile_t transpile_statement(
	transpilation_state_with_indent state,
	variables_t& variables,
	const NodeStructs::MetaType& expected_return_type,
	const NodeStructs::Statement<type_context>& statement
);

transpile_t transpile_statement(
	transpilation_state_with_indent state,
	variables_t& variables,
	const NodeStructs::MetaType& expected_return_type,
	const NodeStructs::Statement<top_level_context>& statement
);
