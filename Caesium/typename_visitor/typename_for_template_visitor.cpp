#include "../core/toCPP.hpp"

using T = typename_for_template_visitor;
using R = T::R;

R T::operator()(const NodeStructs::BaseTypename& t) {
	return t.type;
}

R T::operator()(const NodeStructs::NamespacedTypename& t) {
	return operator()(t.name_space.get()) + "__" + t.name_in_name_space;
}

R T::operator()(const NodeStructs::TemplatedTypename& t) {
	auto args = t.templated_with
		| std::views::transform([&](auto&& e) { return word_typename_or_expression_for_template(e); });
	std::stringstream ss;
	ss << operator()(t.type.get()) << "__";
	for (const auto& arg : args)
		ss << "_" << arg;
	return ss.str();
}

R T::operator()(const NodeStructs::OptionalTypename& t) {
	return "Opt_" + operator()(t.type.get()) + "_";
}

R T::operator()(const NodeStructs::TupleTypename& t) {
	auto args = t.members
		| std::views::transform([&](auto&& e) { return operator()(e); });
	std::stringstream ss;
	ss << "tuple_";
	for (const auto& arg : args)
		ss << "_" << arg;
	return ss.str();
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
	throw;
}
