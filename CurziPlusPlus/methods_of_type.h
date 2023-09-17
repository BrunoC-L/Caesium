#pragma once
#include "node_structs.h"

struct MethodOfTypeTemplateInstance {
	const NodeStructs::Function* templated_function;
	const NodeStructs::TemplateArguments* function_template_argument_names;
	const std::vector<NodeStructs::TypeOrTypeTemplateInstance>* template_arguments_passed_in;
};

using MethodOfTypeOrOfTypeTemplateInstance = std::variant<
	const NodeStructs::Function*,
	MethodOfTypeTemplateInstance
>;

std::vector<MethodOfTypeOrOfTypeTemplateInstance> methods_of_type(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::Type&);
std::vector<MethodOfTypeOrOfTypeTemplateInstance> methods_of_type(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::TypeTemplateInstance&);
std::vector<MethodOfTypeOrOfTypeTemplateInstance> methods_of_type(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::TypeOrTypeTemplateInstance&);
