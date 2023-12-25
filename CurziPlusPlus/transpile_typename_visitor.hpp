#pragma once
#include "toCPP.hpp"
#include "typename_visitor.hpp"

struct transpile_typename_visitor : TypenameVisitor<transpile_typename_visitor> {
	using TypenameVisitor<transpile_typename_visitor>::operator();

	variables_t& variables;
	const Named& named;

	using R = transpile_t;

	R operator()(const NodeStructs::BaseTypename& t);
	R operator()(const NodeStructs::NamespacedTypename& t);
	R operator()(const NodeStructs::TemplatedTypename& t);
	R operator()(const NodeStructs::UnionTypename& t);
};
