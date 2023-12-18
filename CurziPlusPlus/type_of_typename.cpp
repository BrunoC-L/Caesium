#include "toCPP.h"
#include "type_of_typename.h"
#include "type_of_expr.h"
#include "methods_of_type.h"
#include <unordered_map>

const NodeStructs::Template<NodeStructs::Type>* type_template_of_typename(
	std::map<std::string, std::vector<NodeStructs::TypeVariant>>& variables,
	const Named& named,
	const NodeStructs::BaseTypename& type
) {
	if (named.type_templates.contains(type.type))
		return named.type_templates.at(type.type);
	auto err = "Missing type " + type.type;
	throw std::runtime_error(err);
}

const NodeStructs::Template<NodeStructs::Type>* type_template_of_typename(
	std::map<std::string, std::vector<NodeStructs::TypeVariant>>& variables,
	const Named& named,
	const NodeStructs::NamespacedTypename& type
) {
	auto err = "Missing type ";
	throw std::runtime_error(err);
}

const NodeStructs::Template<NodeStructs::Type>* type_template_of_typename(
	std::map<std::string, std::vector<NodeStructs::TypeVariant>>& variables,
	const Named& named,
	const NodeStructs::TemplatedTypename& type
) {
	const auto& templated = type_template_of_typename(variables, named, type.type.get());
	auto err = "Missing type ";
	throw std::runtime_error(err);
}

const NodeStructs::Template<NodeStructs::Type>* type_template_of_typename(
	std::map<std::string, std::vector<NodeStructs::TypeVariant>>& variables,
	const Named& named,
	const NodeStructs::UnionTypename& type
) {
	throw;
}

const NodeStructs::Template<NodeStructs::Type>* type_template_of_typename(
	std::map<std::string, std::vector<NodeStructs::TypeVariant>>& variables,
	const Named& named,
	const NodeStructs::Typename& type
) {
	throw;
	/*return std::visit(
		[&](const auto& t) {
			auto s = transpile(variables, named, t);
			return type_template_of_typename(variables, named, t);
		},
		type
	);*/
}

NodeStructs::TypeVariant type_of_typename(
	std::map<std::string, std::vector<NodeStructs::TypeVariant>>& variables,
	const Named& named,
	const NodeStructs::BaseTypename& type
) {
	if (named.types.contains(type.type))
		return NodeStructs::TypeVariant{ named.types.at(type.type) };
	auto err = "Missing type " + type.type;
	throw std::runtime_error(err);
}

NodeStructs::TypeVariant type_of_typename(
	std::map<std::string, std::vector<NodeStructs::TypeVariant>>& variables,
	const Named& named,
	const NodeStructs::NamespacedTypename& type
) {
	auto err = "Missing type ";
	throw std::runtime_error(err);
}

NodeStructs::TypeVariant type_of_typename(
	std::map<std::string, std::vector<NodeStructs::TypeVariant>>& variables,
	const Named& named,
	const NodeStructs::TemplatedTypename& type
) {
	return NodeStructs::TypeVariant{ NodeStructs::TypeTemplateInstance{
		type_template_of_typename(variables, named, type.type.get()),
		type.templated_with
			| std::views::transform([&](const auto& e) { return type_of_typename_v(variables, named, e); })
			| to_vec()
	} };
}

NodeStructs::TypeVariant type_of_typename(
	std::map<std::string, std::vector<NodeStructs::TypeVariant>>& variables,
	const Named& named,
	const NodeStructs::UnionTypename& type
) {
	return NodeStructs::TypeVariant{ NodeStructs::TypeUnion{
		type.ors
			| std::views::transform([&](const auto& e) { return type_of_typename_v(variables, named, e); })
			| to_vec()
	} };
}

NodeStructs::TypeVariant type_of_typename_v(
	std::map<std::string, std::vector<NodeStructs::TypeVariant>>& variables,
	const Named& named,
	const NodeStructs::Typename& type
) {
	return std::visit(
		[&](const auto& t) {
			return type_of_typename(variables, named, t);
		},
		type.value
	);
}