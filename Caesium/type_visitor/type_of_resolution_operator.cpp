#include "../core/toCPP.hpp"

using T = type_of_resolution_operator_visitor;
using R = T::R;

R T::operator()(const NodeStructs::Type& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const NodeStructs::PrimitiveType& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const NodeStructs::FunctionType& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const NodeStructs::InterfaceType& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const NodeStructs::NamespaceType& nst) {
	const auto& ns = nst.name_space.get();
	if (auto it = ns.types.find(accessed); it != ns.types.end()) {
		const auto& types = it->second;
		if (types.size() != 1)
			NOT_IMPLEMENTED;
		const auto& type = types.at(0);
		auto opt_e = traverse_type(state, type);
		if (opt_e.has_value())
			return opt_e.value();
		return NodeStructs::MetaType{ copy(type) };
	}
	if (auto it = ns.aliases.find(accessed); it != ns.aliases.end()) {
		auto e_t = type_of_typename(state, variables, it->second);
		return_if_error(e_t);
		auto opt_e = traverse_type(state, e_t.value());
		if (opt_e.has_value())
			return opt_e.value();
		return std::move(e_t).value();
	}
	if (auto it = ns.interfaces.find(accessed); it != ns.interfaces.end()) {
		const auto& interfaces = it->second;
		if (interfaces.size() != 1)
			NOT_IMPLEMENTED;
		const auto& interface = interfaces.at(0);
		if (!state.state.traversed_interfaces.contains(interface)) {
			state.state.traversed_interfaces.insert(copy(interface));
			state.state.interfaces_to_transpile.insert(copy(interface));
		}
		return NodeStructs::MetaType{ NodeStructs::InterfaceType{ interface } };
	}
	if (auto it = ns.namespaces.find(accessed); it != ns.namespaces.end())
		return NodeStructs::MetaType{ NodeStructs::NamespaceType{ it->second } };
	return error{ "user error" , "Missing type `" + accessed + "` in namespace `" + ns.name + "`" };
}

R T::operator()(const NodeStructs::Builtin& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const NodeStructs::UnionType& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const NodeStructs::TemplateType& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const NodeStructs::EnumType& e) {
	for (const auto& val : e.enum_.get().values)
		if (val == accessed)
			return NodeStructs::MetaType{ NodeStructs::EnumValueType{.enum_ = e.enum_, .value = accessed } };
	return error{ "user error" , "Missing enum value `" + accessed + "` in enum `" + e.enum_.get().name + "`" };
}

R T::operator()(const NodeStructs::EnumValueType& tmpl) {
	NOT_IMPLEMENTED;
}

R T::operator()(const NodeStructs::OptionalType& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const NodeStructs::AggregateType& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const NodeStructs::Vector& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const NodeStructs::VectorType& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const NodeStructs::Set& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const NodeStructs::SetType& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const NodeStructs::Map& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const NodeStructs::MapType& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const NodeStructs::TypeList& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const NodeStructs::TypeListType& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const NodeStructs::TypeToken& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const NodeStructs::CompileTimeType& t) {
	NOT_IMPLEMENTED;
}
