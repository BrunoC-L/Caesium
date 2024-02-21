#pragma once
#include "../core/toCPP.hpp"
#include "typename_visitor.hpp"

struct transpile_typename_visitor : TypenameVisitor<transpile_typename_visitor> {
	using TypenameVisitor<transpile_typename_visitor>::operator();

	transpilation_state_with_indent state;

	using R = transpile_t;

	R operator()(const NodeStructs::BaseTypename& t);
	R operator()(const NodeStructs::NamespacedTypename& t);
	R operator()(const NodeStructs::TemplatedTypename& t);
	R operator()(const NodeStructs::UnionTypename& t);
};
