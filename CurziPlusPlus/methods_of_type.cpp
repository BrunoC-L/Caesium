//#include "toCPP.h"
//#include "type_of_typename.h"
//#include "type_of_expr.h"
//#include "methods_of_type.h"
//
//
//std::vector<MethodOfTypeOrOfTypeTemplateInstance> methods_of_type(
//	std::map<std::string, std::vector<NodeStructs::TypeVariant>>& variables,
//	const Named& named,
//	const NodeStructs::Type* t
//) {
//	throw;
//}
//
//std::vector<MethodOfTypeOrOfTypeTemplateInstance> methods_of_type(
//	std::map<std::string, std::vector<NodeStructs::TypeVariant>>& variables,
//	const Named& named,
//	const NodeStructs::TypeTemplateInstance& t
//) {
//	return t.type_template->templated.methods
//		| std::views::transform([&](const auto& method_template) {
//			return MethodOfTypeOrOfTypeTemplateInstance{ MethodOfTypeTemplateInstance{ &method_template, &t.type_template->arguments, &t.template_arguments } };
//		})
//		| to_vec();
//}
//
//std::vector<MethodOfTypeOrOfTypeTemplateInstance> methods_of_type(
//	std::map<std::string, std::vector<NodeStructs::TypeVariant>>& variables,
//	const Named& named,
//	const NodeStructs::Aggregate& t
//) {
//	throw;
//}
//
//std::vector<MethodOfTypeOrOfTypeTemplateInstance> methods_of_type(
//	std::map<std::string, std::vector<NodeStructs::TypeVariant>>& variables,
//	const Named& named,
//	const NodeStructs::TypeType& t
//) {
//	throw;
//}
//
//std::vector<MethodOfTypeOrOfTypeTemplateInstance> methods_of_type(
//	std::map<std::string, std::vector<NodeStructs::TypeVariant>>& variables,
//	const Named& named,
//	const NodeStructs::TypeVariant& t
//) {
//	return std::visit(
//		overload(
//			[&](const auto& t) {
//				return methods_of_type(variables, named, t);
//			},
//			[&](const NodeStructs::TypeVariant& e) {
//				static_assert(
//					!sizeof(std::remove_cvref_t<decltype(e)>*), 
//					"Overload set is missing support for a type held in the variant."
//					); 
//				/* requires P2741R3 user-generated static_assert messages
//				static_assert(
//					false,
//					std::format("Overload set is missing support for a type held in the variant. see {}.", std::source_location::current())
//				);*/
//			}
//		),
//		t
//	);
//}
