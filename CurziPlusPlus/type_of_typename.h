#pragma once

struct Named;

const NodeStructs::Template<NodeStructs::Type>* type_template_of_typename(
	std::map<std::string, std::vector<NodeStructs::TypeVariant>>& variables,
	const Named& named,
	const NodeStructs::BaseTypename& type
);

const NodeStructs::Template<NodeStructs::Type>* type_template_of_typename(
	std::map<std::string, std::vector<NodeStructs::TypeVariant>>& variables,
	const Named& named,
	const NodeStructs::NamespacedTypename& type
);

const NodeStructs::Template<NodeStructs::Type>* type_template_of_typename(
	std::map<std::string, std::vector<NodeStructs::TypeVariant>>& variables,
	const Named& named,
	const NodeStructs::TemplatedTypename& type
);

const NodeStructs::Template<NodeStructs::Type>* type_template_of_typename(
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
	const NodeStructs::Typename& type
);
