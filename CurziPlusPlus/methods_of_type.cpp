#include "methods_of_type.h"


std::vector<MethodOfTypeOrOfTypeTemplateInstance> get_methods_of_type(const NodeStructs::Type&) {
	return {};
}

std::vector<MethodOfTypeOrOfTypeTemplateInstance> get_methods_of_type(const NodeStructs::TypeTemplateInstance&) {
	return {};
}

std::vector<MethodOfTypeOrOfTypeTemplateInstance> get_methods_of_type(const NodeStructs::TypeOrTypeTemplateInstance&) {
	return {};
}
