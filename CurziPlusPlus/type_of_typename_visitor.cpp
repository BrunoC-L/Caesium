#include "type_of_typename_visitor.hpp"
#include "type_template_of_typename_visitor.hpp"

using T = type_of_typename_visitor;
using R = T::R;

R T::operator()(const NodeStructs::BaseTypename& t) {
	if (auto it = state.named.types.find(t.type); it != state.named.types.end()) {
		const auto& e = *it->second;
		if (!state.traversed_types.contains(e)) {
			state.traversed_types.insert(e);
			auto t = transpile(state, e);
			if (!t.has_value())
				return std::unexpected{ t.error() };
			state.transpile_in_reverse_order.push_back(std::move(t).value());
		}
		return NodeStructs::TypeCategory{ *state.named.types.at(t.type) };
	}
	auto err = "Missing type " + t.type;
	throw std::runtime_error(err);
}

R T::operator()(const NodeStructs::NamespacedTypename& t) {
	throw;
}

R T::operator()(const NodeStructs::TemplatedTypename& t) {
	auto tmpl = type_template_of_typename_visitor{ {}, state }(t.type.get());
	if (!tmpl.has_value())
		return std::unexpected{ tmpl.error() };
	std::vector<NodeStructs::TypeCategory> v;
	v.reserve(t.templated_with.size());
	for (const auto& e : t.templated_with) {
		auto exp = operator()(e);
		if (!exp.has_value())
			return std::unexpected{ exp.error() };
		v.push_back(exp.value());
	}
	return NodeStructs::TypeCategory{ NodeStructs::TypeTemplateInstanceType{
		tmpl.value(),
		std::move(v)
	} };
}

R T::operator()(const NodeStructs::UnionTypename& t) {
	std::vector<NodeStructs::TypeCategory> v;
	v.reserve(t.ors.size());
	for (const auto& e : t.ors) {
		auto exp = operator()(e);
		if (!exp.has_value())
			return std::unexpected{ exp.error() };
		v.push_back(exp.value());
	}
	return NodeStructs::TypeCategory{ NodeStructs::UnionType{ std::move(v) } };
}
