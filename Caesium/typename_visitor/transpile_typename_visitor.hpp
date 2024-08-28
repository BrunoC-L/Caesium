#pragma once
#include "../core/toCPP.hpp"
#include "typename_visitor.hpp"

struct transpile_typename_visitor : TypenameVisitor<transpile_typename_visitor> {
	using TypenameVisitor<transpile_typename_visitor>::operator();

	transpilation_state_with_indent state;

	using R = transpile_t;

	TypenameVisitorDeclarations
};

transpile_t transpile_typename(
	transpilation_state_with_indent state,
	const auto& tn
) {
	return transpile_typename_visitor{ {}, state }(tn);
}
