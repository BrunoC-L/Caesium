#include "type_template_of_typename_visitor.hpp"

using T = type_template_of_typename_visitor;
using R = T::R;

R T::operator()(const NodeStructs::BaseTypename& t) {
	if (state.state.named.type_templates.contains(t.type))
		return *state.state.named.type_templates.at(t.type);
	auto err = "Missing type " + t.type;
	throw std::runtime_error(err);
}

R T::operator()(const NodeStructs::NamespacedTypename& t) {
	throw;
}

R T::operator()(const NodeStructs::TemplatedTypename& t) {
	const auto& templated = operator()(t.type);
	throw;
	/*return NodeStructs::TypeCategory{ NodeStructs::TypeTemplateInstanceType{
		::template_type_of_typename_visitor{ {}, state }(type.type.get()),
		type.templated_with
			| std::views::transform([&](const auto& e) { return type_of_typename_visitor{ {}, state }(e); })
			| to_vec()
	} };*/
}

R T::operator()(const NodeStructs::UnionTypename& t) {
	throw;
}
