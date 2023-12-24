#include "toCPP.h"
#include "type_of_typename.h"
#include "type_of_expr.h"
#include <unordered_map>

std::reference_wrapper<const NodeStructs::Template<NodeStructs::Type>> type_template_of_typename(
	variables_t& variables,
	const Named& named,
	const NodeStructs::BaseTypename& type
) {
	if (named.type_templates.contains(type.type))
		return *named.type_templates.at(type.type);
	auto err = "Missing type " + type.type;
	throw std::runtime_error(err);
}

std::reference_wrapper<const NodeStructs::Template<NodeStructs::Type>> type_template_of_typename(
	variables_t& variables,
	const Named& named,
	const NodeStructs::NamespacedTypename& type
) {
	auto err = "Missing type ";
	throw std::runtime_error(err);
}

std::reference_wrapper<const NodeStructs::Template<NodeStructs::Type>> type_template_of_typename(
	variables_t& variables,
	const Named& named,
	const NodeStructs::TemplatedTypename& type
) {
	const auto& templated = type_template_of_typename_v(variables, named, type.type.get());
	auto err = "Missing type ";
	throw std::runtime_error(err);
}

std::reference_wrapper<const NodeStructs::Template<NodeStructs::Type>> type_template_of_typename(
	variables_t& variables,
	const Named& named,
	const NodeStructs::UnionTypename& type
) {
	throw;
}

std::reference_wrapper<const NodeStructs::Template<NodeStructs::Type>> type_template_of_typename_v(
	variables_t& variables,
	const Named& named,
	const NodeStructs::Typename& type
) {
	return std::visit(
		[&](const auto& t) {
			auto s = transpile(variables, named, t);
			return type_template_of_typename(variables, named, t);
		},
		type.value
	);
}

NodeStructs::TypeCategory type_of_typename(
	variables_t& variables,
	const Named& named,
	const NodeStructs::BaseTypename& type
) {
	if (named.types.contains(type.type))
		return NodeStructs::TypeCategory{ *named.types.at(type.type) };
	auto err = "Missing type " + type.type;
	throw std::runtime_error(err);
}

NodeStructs::TypeCategory type_of_typename(
	variables_t& variables,
	const Named& named,
	const NodeStructs::NamespacedTypename& type
) {
	auto err = "Missing type ";
	throw std::runtime_error(err);
}

NodeStructs::TypeCategory type_of_typename(
	variables_t& variables,
	const Named& named,
	const NodeStructs::TemplatedTypename& type
) {
	return NodeStructs::TypeCategory{ NodeStructs::TypeTemplateInstance{
		type_template_of_typename_v(variables, named, type.type.get()),
		type.templated_with
			| std::views::transform([&](const auto& e) { return type_of_typename_v(variables, named, e); })
			| to_vec()
	} };
}

NodeStructs::TypeCategory type_of_typename(
	variables_t& variables,
	const Named& named,
	const NodeStructs::UnionTypename& type
) {
	return NodeStructs::TypeCategory{ NodeStructs::UnionType{
		type.ors
			| std::views::transform([&](const auto& e) { return type_of_typename_v(variables, named, e); })
			| to_vec()
	} };
}

NodeStructs::TypeCategory type_of_typename_v(
	variables_t& variables,
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