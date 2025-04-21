#include "../core/toCPP.hpp"

using T = type_of_resolution_operator_visitor;
using R = T::R;

R T::operator()(const Realised::Type& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const Realised::PrimitiveType& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const Realised::FunctionType& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const Realised::InterfaceType& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const Realised::NamespaceType& nst) {
	const auto& ns = nst.name_space.get();
	if (auto it = ns.types.find(accessed); it != ns.types.end()) {
		const auto& types = it->second;
		if (types.size() != 1)
			NOT_IMPLEMENTED;
		const auto& type = types.at(0);
		NOT_IMPLEMENTED;
		/*auto opt_e = traverse_type(state, type);
		if (opt_e.has_value())
			return opt_e.value();
		return Realised::MetaType{ copy(type) };*/
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
		NOT_IMPLEMENTED;
		/*if (!state.state.interfaces_traversal.traversed.contains(interface)) {
			state.state.interfaces_traversal.traversed.insert(copy(interface));
			state.state.interfaces_to_transpile.insert(copy(interface));
		}
		return Realised::MetaType{ Realised::InterfaceType{ interface } };*/
	}
	NOT_IMPLEMENTED;
	/*if (auto it = ns.namespaces.find(accessed); it != ns.namespaces.end())
		return Realised::MetaType{ Realised::NamespaceType{ it->second } };*/
	return error{ "user error" , "Missing type `" + accessed + "` in namespace `" + ns.name + "`" };
}

R T::operator()(const Realised::Builtin& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const Realised::UnionType& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const Realised::TemplateType& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const Realised::EnumType& e) {
	NOT_IMPLEMENTED;
	/*for (const auto& val : e.enum_.get().values)
		if (val == accessed)
			return Realised::MetaType{ Realised::EnumValueType{.enum_ = e.enum_, .value = accessed } };
	return error{ "user error" , "Missing enum value `" + accessed + "` in enum `" + e.enum_.get().name + "`" };*/
}

R T::operator()(const Realised::EnumValueType& tmpl) {
	NOT_IMPLEMENTED;
}

R T::operator()(const Realised::OptionalType& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const Realised::AggregateType& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const Realised::VectorType& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const Realised::SetType& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const Realised::MapType& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const Realised::TypeListType& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const Realised::CompileTimeType& t) {
	NOT_IMPLEMENTED;
}
