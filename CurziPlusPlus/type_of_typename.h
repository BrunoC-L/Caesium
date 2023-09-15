#pragma once

struct Named;

NodeStructs::Template<const NodeStructs::Type*> type_template_of_typename(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::BaseTypename& type
);

NodeStructs::Template<const NodeStructs::Type*> type_template_of_typename(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::NamespacedTypename& type
);

NodeStructs::Template<const NodeStructs::Type*> type_template_of_typename(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::TemplatedTypename& type
);

NodeStructs::Template<const NodeStructs::Type*> type_template_of_typename(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::Typename& type
);

NodeStructs::TypeOrTypeTemplateInstance type_of_typename(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::BaseTypename& type
);

NodeStructs::TypeOrTypeTemplateInstance type_of_typename(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::NamespacedTypename& type
);

NodeStructs::TypeOrTypeTemplateInstance type_of_typename(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::TemplatedTypename& type
);

NodeStructs::TypeOrTypeTemplateInstance type_of_typename(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::Typename& type
);
