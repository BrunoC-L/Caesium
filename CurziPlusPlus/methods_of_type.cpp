#include "toCPP.h"
#include "type_of_typename.h"
#include "type_of_expr.h"
#include "methods_of_type.h"


std::vector<MethodOfTypeOrOfTypeTemplateInstance> methods_of_type(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::Type& t
) {
	throw std::runtime_error("");
	return {};
}

std::vector<MethodOfTypeOrOfTypeTemplateInstance> methods_of_type(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::TypeTemplateInstance& t
) {
	std::vector<MethodOfTypeOrOfTypeTemplateInstance> res{};
	for (const auto& method_template : t.type_template->templated.methods)
		res.push_back(MethodOfTypeTemplateInstance{ &method_template, &t.type_template->arguments, &t.template_arguments });
	
	return res;
}

std::vector<MethodOfTypeOrOfTypeTemplateInstance> methods_of_type(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::TypeOrTypeTemplateInstance& t
) {
	return std::visit([&](const auto& t) { return methods_of_type(variables, named, t); }, t);
}
