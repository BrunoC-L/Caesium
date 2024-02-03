#include "type_of_typename_visitor.hpp"
#include "type_template_of_typename_visitor.hpp"
#include "traverse_type_visitor.hpp"

using T = type_of_typename_visitor;
using R = T::R;

R T::operator()(const NodeStructs::BaseTypename& t) {
	if (auto it = state.state.named.types.find(t.type); it != state.state.named.types.end()) {
		const auto& type = *it->second.back();
		if (!state.state.traversed_types.contains(type)) {
			state.state.traversed_types.insert(type);
			auto t = transpile(state, type);
			return_if_error(t);
			state.state.transpile_in_reverse_order.push_back(std::move(t).value());
		}
		return NodeStructs::UniversalType{ *state.state.named.types.at(t.type).back() };
	}
	if (auto it = state.state.named.type_aliases.find(t.type); it != state.state.named.type_aliases.end()) {
		const auto& type = it->second;
		traverse_type_visitor{ {}, state }(type);
		return it->second;
	}
	auto err = "Missing type " + t.type;
	throw std::runtime_error(err);
}

R T::operator()(const NodeStructs::NamespacedTypename& t) {
	throw;
}

R T::operator()(const NodeStructs::TemplatedTypename& t) {
	return type_template_of_typename_visitor{ {}, state, t.templated_with }(t.type.get());
}

R T::operator()(const NodeStructs::UnionTypename& t) {
	std::vector<NodeStructs::UniversalType> v;
	v.reserve(t.ors.size());
	for (const auto& e : t.ors) {
		auto exp = operator()(e);
		return_if_error(exp);
		v.push_back(exp.value());
	}
	return NodeStructs::UniversalType{ NodeStructs::UnionType{ std::move(v) } };
}
