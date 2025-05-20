#pragma once
#include "../core/toCPP.hpp"
#include "typename_visitor.hpp"

struct realise_typename_visitor : TypenameVisitor<realise_typename_visitor> {
	using TypenameVisitor<realise_typename_visitor>::operator();

	transpilation_state_with_indent state;

	using R = std::optional<error>;

	TypenameVisitorDeclarations;
};

std::optional<error> realise_typename(
	transpilation_state_with_indent state,
	const auto& type
) {
	return realise_typename_visitor{ {}, state }(type);
}
