#include "type_of_typename_visitor.hpp"
#include "type_template_of_typename_visitor.hpp"

using T = type_of_typename_visitor;
using R = T::R;

R T::operator()(const NodeStructs::BaseTypename& t) {
	if (named.types.contains(t.type))
		return NodeStructs::TypeCategory{ named.types.at(t.type).back() };
	auto err = "Missing type " + t.type;
	throw std::runtime_error(err);
}

R T::operator()(const NodeStructs::NamespacedTypename& t) {
	throw;
}

R T::operator()(const NodeStructs::TemplatedTypename& t) {
	return NodeStructs::TypeCategory{ NodeStructs::TypeTemplateInstanceType{
		type_template_of_typename_visitor{ {}, variables, named }(t.type.get()),
		t.templated_with
			| std::views::transform([&](const auto& e) { return operator()(e); })
			| to_vec()
	} };
}

R T::operator()(const NodeStructs::UnionTypename& t) {
	return NodeStructs::TypeCategory{ NodeStructs::UnionType{
		t.ors
			| std::views::transform([&](const auto& e) { return operator()(e); })
			| to_vec()
	} };
}