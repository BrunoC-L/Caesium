#pragma once
#include "../core/toCPP.hpp"
#include "builtin_visitor.hpp"

struct transpile_builtin_call_with_args_visitor : BuiltinVisitor<transpile_builtin_call_with_args_visitor> {
	using BuiltinVisitor<transpile_builtin_call_with_args_visitor>::operator();

	transpilation_state_with_indent state;
	variables_t& variables;
	const std::vector<NodeStructs::FunctionArgument>& args;

	using R = transpile_expression_information_t;

	BuiltinVisitorDeclarations
};

transpile_builtin_call_with_args_visitor::R transpile_builtin_call_with_args(
	transpilation_state_with_indent state,
	variables_t& variables,
	const std::vector<NodeStructs::FunctionArgument>& arguments,
	const auto& builtin
) {
	return transpile_builtin_call_with_args_visitor{ {}, state, variables, arguments }(builtin);
}
