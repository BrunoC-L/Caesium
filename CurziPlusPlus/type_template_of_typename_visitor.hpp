#pragma once
#include "toCPP.hpp"
#include "typename_visitor.hpp"

struct type_template_of_typename_visitor : TypenameVisitor<type_template_of_typename_visitor> {
	using TypenameVisitor<type_template_of_typename_visitor>::operator();

	transpilation_state_with_indent state;

	using R = expected<std::reference_wrapper<const NodeStructs::Template<NodeStructs::Type>>>;

	R operator()(const NodeStructs::BaseTypename& t);
	R operator()(const NodeStructs::NamespacedTypename& t);
	R operator()(const NodeStructs::TemplatedTypename& t);
	R operator()(const NodeStructs::UnionTypename& t);
};
