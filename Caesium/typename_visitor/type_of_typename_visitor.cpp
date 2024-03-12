#include "../core/toCPP.hpp"

using T = type_of_typename_visitor;
using R = T::R;

R T::operator()(const NodeStructs::BaseTypename& t) {
	if (auto it = state.state.named.types.find(t.type); it != state.state.named.types.end()) {
		const auto& type = *it->second.back();
		auto opt_e = traverse_type(state, type);
		if (opt_e.has_value())
			return opt_e.value();
		return NodeStructs::MetaType{ NodeStructs::MetaType{ type } };
	}
	if (auto it = state.state.named.type_aliases.find(t.type); it != state.state.named.type_aliases.end()) {
		const auto& type = it->second;
		auto opt_e = traverse_type(state, type);
		if (opt_e.has_value())
			return opt_e.value();
		return it->second;
	}
	if (auto it = state.state.named.interfaces.find(t.type); it != state.state.named.interfaces.end()) {
		const auto& interface = *it->second.back();
		if (!state.state.traversed_interfaces.contains(interface)) {
			state.state.traversed_interfaces.insert(interface);
			auto t = transpile(state, interface);
			return_if_error(t);
			state.state.transpile_in_reverse_order.push_back(std::move(t).value());
		}
		return NodeStructs::MetaType{ NodeStructs::InterfaceType{ interface } };
	}
	if (auto it = state.state.named.namespaces.find(t.type); it != state.state.named.namespaces.end()) {
		const auto& ns = *it->second.back();
		return NodeStructs::MetaType{ NodeStructs::NamespaceType{ ns } };
	}
	return error{ "user error" , "Missing type `" + t.type + "`"};
}

R T::operator()(const NodeStructs::NamespacedTypename& t) {
	auto e_space_t = operator()(t.name_space);
	return_if_error(e_space_t);
	const auto& space_t = e_space_t.value();
	return type_of_resolution_operator(state, space_t, t.name_in_name_space);
}

R T::operator()(const NodeStructs::TemplatedTypename& t) {
	return type_template_of_typename(state, t.templated_with, t.type.get());
}

R T::operator()(const NodeStructs::UnionTypename& t) {
	std::vector<NodeStructs::MetaType> v;
	v.reserve(t.ors.size());
	for (const auto& e : t.ors) {
		auto exp = operator()(e);
		return_if_error(exp);
		v.push_back(exp.value());
	}
	return NodeStructs::MetaType{ NodeStructs::UnionType{ std::move(v) } };
}
