#pragma once

struct Named;

std::reference_wrapper<const NodeStructs::Template<NodeStructs::Type>> type_template_of_typename(
	std::map<std::string, std::vector<NodeStructs::TypeVariant>>& variables,
	const Named& named,
	const NodeStructs::BaseTypename& type
);

std::reference_wrapper<const NodeStructs::Template<NodeStructs::Type>> type_template_of_typename(
	std::map<std::string, std::vector<NodeStructs::TypeVariant>>& variables,
	const Named& named,
	const NodeStructs::NamespacedTypename& type
);

std::reference_wrapper<const NodeStructs::Template<NodeStructs::Type>> type_template_of_typename(
	std::map<std::string, std::vector<NodeStructs::TypeVariant>>& variables,
	const Named& named,
	const NodeStructs::TemplatedTypename& type
);

std::reference_wrapper<const NodeStructs::Template<NodeStructs::Type>> type_template_of_typename(
	std::map<std::string, std::vector<NodeStructs::TypeVariant>>& variables,
	const Named& named,
	const NodeStructs::UnionTypename& type
);

std::reference_wrapper<const NodeStructs::Template<NodeStructs::Type>> type_template_of_typename_v(
	std::map<std::string, std::vector<NodeStructs::TypeVariant>>& variables,
	const Named& named,
	const NodeStructs::Typename& type
);

NodeStructs::TypeVariant type_of_typename(
	std::map<std::string, std::vector<NodeStructs::TypeVariant>>& variables,
	const Named& named,
	const NodeStructs::BaseTypename& type
);

NodeStructs::TypeVariant type_of_typename(
	std::map<std::string, std::vector<NodeStructs::TypeVariant>>& variables,
	const Named& named,
	const NodeStructs::NamespacedTypename& type
);

NodeStructs::TypeVariant type_of_typename(
	std::map<std::string, std::vector<NodeStructs::TypeVariant>>& variables,
	const Named& named,
	const NodeStructs::TemplatedTypename& type
);

NodeStructs::TypeVariant type_of_typename(
	std::map<std::string, std::vector<NodeStructs::TypeVariant>>& variables,
	const Named& named,
	const NodeStructs::UnionTypename& type
);

NodeStructs::TypeVariant type_of_typename_v(
	std::map<std::string, std::vector<NodeStructs::TypeVariant>>& variables,
	const Named& named,
	const NodeStructs::Typename& type
);
