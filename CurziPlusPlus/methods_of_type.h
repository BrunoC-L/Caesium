#pragma once
#include "node_structs.h"

using MethodOfTypeOrOfTypeTemplateInstance = std::variant<NodeStructs::Function*, std::pair<NodeStructs::Function, NodeStructs::TypeTemplateInstance>>;

std::vector<MethodOfTypeOrOfTypeTemplateInstance> get_methods_of_type(const NodeStructs::Type&);
std::vector<MethodOfTypeOrOfTypeTemplateInstance> get_methods_of_type(const NodeStructs::TypeTemplateInstance&);
std::vector<MethodOfTypeOrOfTypeTemplateInstance> get_methods_of_type(const NodeStructs::TypeOrTypeTemplateInstance&);
