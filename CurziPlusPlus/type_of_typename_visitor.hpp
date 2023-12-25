#pragma once
#include "toCPP.h"
#include "typename_visitor.hpp"

struct type_of_typename_visitor : TypenameVisitor<type_of_typename_visitor> {
	using TypenameVisitor<type_of_typename_visitor>::operator();

	variables_t& variables;
	const Named& named;

	using R = NodeStructs::TypeCategory;

	R operator()(const NodeStructs::BaseTypename& t);
	R operator()(const NodeStructs::NamespacedTypename& t);
	R operator()(const NodeStructs::TemplatedTypename& t);
	R operator()(const NodeStructs::UnionTypename& t);
};
