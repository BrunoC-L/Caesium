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
	if (auto it = ns.types.find(accessed); it != ns.types.end()) {
		const auto& types = it->second;
		if (types.size() != 1)
			throw;
		const auto& type = types.at(0);
		auto opt_e = traverse_type(state, type);
		if (opt_e.has_value())
			return opt_e.value();
		return NodeStructs::MetaType{ type };
	}
	if (auto it = ns.aliases.find(accessed); it != ns.aliases.end()) {
		const auto& e_t = type_of_typename(state, it->second);
		return_if_error(e_t);
		auto opt_e = traverse_type(state, e_t.value());
		if (opt_e.has_value())
			return opt_e.value();
		return e_t.value();
	}
	if (auto it = ns.interfaces.find(accessed); it != ns.interfaces.end()) {
		const auto& interfaces = it->second;
		if (interfaces.size() != 1)
			throw;
		const auto& interface = interfaces.at(0);
		if (!state.state.traversed_interfaces.contains(interface)) {
			state.state.traversed_interfaces.insert(interface);
			state.state.interfaces_to_transpile.insert(interface);
		}
		return NodeStructs::MetaType{ NodeStructs::InterfaceType{ interface } };
	}
	if (auto it = ns.namespaces.find(accessed); it != ns.namespaces.end())
		return NodeStructs::MetaType{ NodeStructs::NamespaceType{ it->second } };
	return error{ "user error" , "Missing type `" + accessed + "` in namespace `" + ns.name + "`" };
}

R T::operator()(const NodeStructs::Builtin& t) {
	throw;
}

R T::operator()(const NodeStructs::UnionType& t) {
	throw;
}

R T::operator()(const NodeStructs::TemplateType& t) {
	throw;
}

R T::operator()(const NodeStructs::Enum& tmpl) {
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