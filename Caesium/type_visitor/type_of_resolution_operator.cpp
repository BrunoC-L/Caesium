#include "../core/toCPP.hpp"

using T = type_of_resolution_operator_visitor;
using R = T::R;

R T::operator()(const std::reference_wrapper<const NodeStructs::Type>& t) {
	throw;
}

R T::operator()(const NodeStructs::PrimitiveType& t) {
	throw;
}

R T::operator()(const NodeStructs::FunctionType& t) {
	throw;
}

R T::operator()(const NodeStructs::InterfaceType& t) {
	throw;
}

R T::operator()(const NodeStructs::NamespaceType& nst) {
	const auto& ns = nst.name_space.get();
	if (auto it = std::find_if(ns.types.begin(), ns.types.end(), [&](const auto& t) { return t.name == accessed; }); it != ns.types.end()) {
		const auto& type = *it;
		auto opt_e = traverse_type(state, type);
		if (opt_e.has_value())
			return opt_e.value();
		return NodeStructs::MetaType{ type };
	}
	if (auto it = std::find_if(ns.aliases.begin(), ns.aliases.end(), [&](const auto& t) { return t.aliasFrom == accessed; }); it != ns.aliases.end()) {
		const auto& e_t = type_of_typename(state, it->aliasTo);
		return_if_error(e_t);
		auto opt_e = traverse_type(state, e_t.value());
		if (opt_e.has_value())
			return opt_e.value();
		return e_t.value();
	}
	if (auto it = std::find_if(ns.interfaces.begin(), ns.interfaces.end(), [&](const auto& t) { return t.name == accessed; }); it != ns.interfaces.end()) {
		const auto& interface = *it;
		if (!state.state.traversed_interfaces.contains(interface)) {
			state.state.traversed_interfaces.insert(interface);
			auto t = transpile(state, interface);
			return_if_error(t);
			state.state.transpile_in_reverse_order.push_back(std::move(t).value());
		}
		return NodeStructs::MetaType{ NodeStructs::InterfaceType{ interface } };
	}
	if (auto it = std::find_if(ns.namespaces.begin(), ns.namespaces.end(), [&](const auto& t) { return t.name == accessed; }); it != ns.namespaces.end()) {
		const auto& ns = *it;
		return NodeStructs::MetaType{ NodeStructs::NamespaceType{ ns } };
	}
	return error{ "user error" , "Missing type `" + accessed + "` in namespace `" + ns.name + "`" };
}

R T::operator()(const NodeStructs::UnionType& t) {
	throw;
}

R T::operator()(const NodeStructs::Template& t) {
	throw;
}

R T::operator()(const NodeStructs::Vector& t) {
	throw;
}

R T::operator()(const NodeStructs::VectorType& t) {
	throw;
}

R T::operator()(const NodeStructs::Set& t) {
	throw;
}

R T::operator()(const NodeStructs::SetType& t) {
	throw;
}

R T::operator()(const NodeStructs::Map& t) {
	throw;
}

R T::operator()(const NodeStructs::MapType& t) {
	throw;
}