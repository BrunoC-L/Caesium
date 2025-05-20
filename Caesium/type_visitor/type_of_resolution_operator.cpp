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
	if (auto it = find_by_name(ns.types, accessed); it != ns.types.end()) {
		const auto& type = *it;
		auto e_or_t = realise_type(state, type);
		return_if_error(e_or_t);
		return Realised::MetaType{ std::move(e_or_t).value() };
	}
	if (auto it = find_by_name(ns.aliases, accessed); it != ns.aliases.end()) {
		if (std::optional<error> err = realise_typename(state, it->aliasTo); err.has_value())
			return err.value();
		auto e_t = type_of_typename(state, variables, it->aliasTo);
		return_if_error(e_t);
		return std::move(e_t).value();
	}
	if (auto it = find_by_name(ns.interfaces, accessed); it != ns.interfaces.end()) {
		const auto& interface = *it;
		NOT_IMPLEMENTED;
		/*if (!state.state.interfaces_traversal.traversed.contains(interface)) {
			state.state.interfaces_traversal.traversed.insert(copy(interface));
			state.state.interfaces_to_transpile.insert(copy(interface));
		}
		return Realised::MetaType{ Realised::InterfaceType{ interface } };*/
	}
	if (auto it = find_by_name(ns.namespaces, accessed); it != ns.namespaces.end())
		return Realised::MetaType{ Realised::NamespaceType{ accessed, *it } };
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
