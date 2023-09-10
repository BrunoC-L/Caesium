#include "toCPP.h"
#include "type_of_typename.h"
#include "type_of_expr.h"

const NodeStructs::Template<NodeStructs::Type>& type_template_of_typename(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::BaseTypename& type
) {
	if (named.type_templates.contains(type.type))
		return *named.type_templates.at(type.type);
	auto err = "Missing type " + type.type;
	throw std::runtime_error(err);
}

const NodeStructs::Template<NodeStructs::Type>& type_template_of_typename(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::NamespacedTypename& type
) {
	auto err = "Missing type ";
	throw std::runtime_error(err);
}

const NodeStructs::Template<NodeStructs::Type>& type_template_of_typename(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::TemplatedTypename& type
) {
	const auto& templated = type_template_of_typename(variables, named, *type.type.get());
	auto err = "Missing type ";
	throw std::runtime_error(err);
}

const NodeStructs::Template<NodeStructs::Type>& type_template_of_typename(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::Typename& type
) {
	return std::visit(
		[&](const auto& t) -> const NodeStructs::Template<NodeStructs::Type>& {
			auto s = transpile(variables, named, type);
			return type_template_of_typename(variables, named, t);
		},
		type
	);
}

NodeStructs::TypeOrTypeTemplateInstance type_of_typename(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::BaseTypename& type
) {
	if (named.types.contains(type.type))
		return named.types.at(type.type);
	auto err = "Missing type " + type.type;
	throw std::runtime_error(err);
}

NodeStructs::TypeOrTypeTemplateInstance type_of_typename(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::NamespacedTypename& type
) {
	auto err = "Missing type ";
	throw std::runtime_error(err);
}

NodeStructs::TypeOrTypeTemplateInstance type_of_typename(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::TemplatedTypename& type
) {
	const auto& templated = type_template_of_typename(variables, named, *type.type.get());
	auto templated_with = std::vector<NodeStructs::TypeOrTypeTemplateInstance>{};
	for (const auto& t : type.templated_with)
		templated_with.push_back(type_of_typename(variables, named, t));
	return NodeStructs::TypeTemplateInstance{
		templated,
		templated_with
	};
}

NodeStructs::TypeOrTypeTemplateInstance type_of_typename(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::Typename& type
) {
	auto s = transpile(variables, named, type);
	return std::visit(
		[&](const auto& t) {
			return type_of_typename(variables, named, t);
		},
		type
	);
}