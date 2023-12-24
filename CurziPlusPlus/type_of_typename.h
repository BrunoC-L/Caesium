#pragma once
#include "toCPP.h"

std::reference_wrapper<const NodeStructs::Template<NodeStructs::Type>> type_template_of_typename(
	variables_t& variables,
	const Named& named,
	const NodeStructs::BaseTypename& type
);

std::reference_wrapper<const NodeStructs::Template<NodeStructs::Type>> type_template_of_typename(
	variables_t& variables,
	const Named& named,
	const NodeStructs::NamespacedTypename& type
);

std::reference_wrapper<const NodeStructs::Template<NodeStructs::Type>> type_template_of_typename(
	variables_t& variables,
	const Named& named,
	const NodeStructs::TemplatedTypename& type
);

std::reference_wrapper<const NodeStructs::Template<NodeStructs::Type>> type_template_of_typename(
	variables_t& variables,
	const Named& named,
	const NodeStructs::UnionTypename& type
);

std::reference_wrapper<const NodeStructs::Template<NodeStructs::Type>> type_template_of_typename_v(
	variables_t& variables,
	const Named& named,
	const NodeStructs::Typename& type
);

NodeStructs::TypeVariant type_of_typename(
	variables_t& variables,
	const Named& named,
	const NodeStructs::BaseTypename& type
);

NodeStructs::TypeVariant type_of_typename(
	variables_t& variables,
	const Named& named,
	const NodeStructs::NamespacedTypename& type
);

NodeStructs::TypeVariant type_of_typename(
	variables_t& variables,
	const Named& named,
	const NodeStructs::TemplatedTypename& type
);

NodeStructs::TypeVariant type_of_typename(
	variables_t& variables,
	const Named& named,
	const NodeStructs::UnionTypename& type
);

NodeStructs::TypeVariant type_of_typename_v(
	variables_t& variables,
	const Named& named,
	const NodeStructs::Typename& type
);
