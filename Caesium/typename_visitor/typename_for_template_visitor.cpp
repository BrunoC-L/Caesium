#include "../core/toCPP.hpp"
#include "typename_for_template_visitor.hpp"

using T = typename_for_template_visitor;
using R = T::R;

R T::operator()(const NodeStructs::BaseTypename& t) {
	return t.type;
}

R T::operator()(const NodeStructs::NamespacedTypename& t) {
	return operator()(t.name_space) + "__" + t.name_in_name_space;
}

R T::operator()(const NodeStructs::TemplatedTypename& t) {
	NOT_IMPLEMENTED;
	/*auto args = t.templated_with
		| std::views::transform([&](auto&& e) { return word_typename_or_expression_for_template(state, variables, e); });
	std::stringstream ss;
	ss << operator()(t.type) << "__";
	for (const auto& arg : args)
		ss << "_" << arg;
	return ss.str();*/
}

R T::operator()(const NodeStructs::OptionalTypename& t) {
	return "Opt_" + operator()(t.type) + "_";
}

R T::operator()(const NodeStructs::UnionTypename& t) {
	auto args = t.ors
		| std::views::transform([&](auto&& e) { return operator()(e); });
	std::stringstream ss;
	ss << "union_";
	for (const auto& arg : args)
		ss << "_" << arg;
	return ss.str();
}

R T::operator()(const NodeStructs::VariadicExpansionTypename& t) {
	NOT_IMPLEMENTED;
}
