#include "toCPP.h"
#include "type_of_typename.h"
#include "type_of_expr.h"
#include "methods_of_type.h"


std::vector<MethodOfTypeOrOfTypeTemplateInstance> methods_of_type(
	std::map<std::string, std::vector<NodeStructs::TypeVariant>>& variables,
	const Named& named,
	const NodeStructs::Type& t
) {
	throw std::runtime_error("");
	return {};
}

std::vector<MethodOfTypeOrOfTypeTemplateInstance> methods_of_type(
	std::map<std::string, std::vector<NodeStructs::TypeVariant>>& variables,
	const Named& named,
	const NodeStructs::TypeTemplateInstance& t
) {
	return t.type_template->templated.methods
		| std::views::transform([&](const auto& method_template) {
			return MethodOfTypeOrOfTypeTemplateInstance{ MethodOfTypeTemplateInstance{ &method_template, &t.type_template->arguments, &t.template_arguments } };
		})
		| std::ranges::to<std::vector>();
}

std::vector<MethodOfTypeOrOfTypeTemplateInstance> methods_of_type(
	std::map<std::string, std::vector<NodeStructs::TypeVariant>>& variables,
	const Named& named,
	const NodeStructs::Aggregate& t
) {
	throw std::runtime_error("");
	return {};
}

std::vector<MethodOfTypeOrOfTypeTemplateInstance> methods_of_type(
	std::map<std::string, std::vector<NodeStructs::TypeVariant>>& variables,
	const Named& named,
	const NodeStructs::TypeVariant& t
) {
	return std::visit([&](const auto& t) { return methods_of_type(variables, named, t); }, t);
}
