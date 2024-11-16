#include "../core/toCPP.hpp"

using T = type_of_typename_visitor;
using R = T::R;

R T::operator()(const NodeStructs::BaseTypename& t) {
	if (t.type == "Int")
		return NodeStructs::MetaType{ NodeStructs::PrimitiveType{ { int{} } } };
	if (t.type == "Bool")
		return NodeStructs::MetaType{ NodeStructs::PrimitiveType{ { bool{} } } };
	if (t.type == "String")
		return NodeStructs::MetaType{ NodeStructs::PrimitiveType{ { std::string{} } } };
	if (t.type == "Char")
		return NodeStructs::MetaType{ NodeStructs::PrimitiveType{ { char{} } } };
	if (t.type == "Void")
		return NodeStructs::MetaType{ NodeStructs::PrimitiveType{ { NodeStructs::void_t{} } } };
	if (t.type == "Floating")
		return NodeStructs::MetaType{ NodeStructs::PrimitiveType{ { double{} } } };
	if (t.type == "None") {
		throw;
		return NodeStructs::MetaType{ NodeStructs::PrimitiveType{ { NodeStructs::empty_optional_t{} } } };
	}

	if (auto it = state.state.global_namespace.types.find(t.type); it != state.state.global_namespace.types.end()) {
		const auto& types = it->second;
		if (types.size() != 1)
			throw;
		const auto& type = types.at(0);
		auto opt_e = traverse_type(state, type);
		if (opt_e.has_value())
			return opt_e.value();
		return NodeStructs::MetaType{ NodeStructs::MetaType{ copy(type) } };
	}
	bool has_f = state.state.global_namespace.functions.find(t.type) != state.state.global_namespace.functions.end();
	bool has_f_using_auto = state.state.global_namespace.functions_using_auto.find(t.type) != state.state.global_namespace.functions_using_auto.end();
	if (has_f || has_f_using_auto)
		return NodeStructs::MetaType{ NodeStructs::FunctionType{
			t.type, state.state.global_namespace
		} };
	if (auto it = state.state.global_namespace.templates.find(t.type); it != state.state.global_namespace.templates.end()) {
		return NodeStructs::MetaType{ NodeStructs::TemplateType{ t.type, state.state.global_namespace } };
	}
	if (auto it = state.state.global_namespace.aliases.find(t.type); it != state.state.global_namespace.aliases.end()) {
		const auto& type_name = it->second;
		auto type = type_of_typename(state, type_name);
		return_if_error(type);
		if (std::optional<error> err = traverse_type(state, type.value()); err.has_value())
			return err.value();
		return std::move(type).value();
	}
	if (auto it = state.state.global_namespace.interfaces.find(t.type); it != state.state.global_namespace.interfaces.end()) {
		const auto& interfaces = it->second;
		if (interfaces.size() != 1)
			throw;
		const auto& interface = interfaces.at(0);
		if (!state.state.traversed_interfaces.contains(interface)) {
			state.state.traversed_interfaces.insert(copy(interface));
			state.state.interfaces_to_transpile.insert(copy(interface));
		}
		return NodeStructs::MetaType{ NodeStructs::InterfaceType{ interface } };
	}
	if (auto it = state.state.global_namespace.namespaces.find(t.type); it != state.state.global_namespace.namespaces.end())
		return NodeStructs::MetaType{ NodeStructs::NamespaceType{ it->second } };
	if (auto it = state.state.global_namespace.builtins.find(t.type); it != state.state.global_namespace.builtins.end())
		return NodeStructs::MetaType{ NodeStructs::Builtin{ t.type } };
	if (auto it = state.state.global_namespace.enums.find(t.type); it != state.state.global_namespace.enums.end()) {
		const auto& enums = it->second;
		if (enums.size() != 1)
			throw;
		const auto& enum_ = enums.at(0);
		if (!state.state.enums_to_transpile.contains(enum_))
			state.state.enums_to_transpile.insert(copy(enum_));
		return NodeStructs::MetaType{ NodeStructs::EnumType{ enum_ } };
	}
	return error{ "user error" , "Missing type `" + t.type + "`"};
}

R T::operator()(const NodeStructs::NamespacedTypename& t) {
	auto e_space_t = operator()(t.name_space);
	return_if_error(e_space_t);
	const auto& space_t = e_space_t.value();
	return type_of_resolution_operator(state, space_t, t.name_in_name_space);
}

R T::operator()(const NodeStructs::TemplatedTypename& tt) {
	std::vector<NodeStructs::WordTypenameOrExpression> templated_with;
	for (const auto& t : tt.templated_with) {
		auto arg = type_of_typename(state, t);
		return_if_error(arg);
		auto tn = typename_of_type(state, arg.value());
		return_if_error(tn);
		templated_with.push_back({ std::move(tn).value() });
	}
	return type_template_of_typename(state, templated_with, tt.type.get());
}

R T::operator()(const NodeStructs::OptionalTypename& t) {
	auto t_or_e = operator()(t.type);
	return_if_error(t_or_e);
	return NodeStructs::MetaType{ NodeStructs::OptionalType{ std::move(t_or_e).value() } };
}

R T::operator()(const NodeStructs::TupleTypename& type) {
	throw;
}

R T::operator()(const NodeStructs::UnionTypename& t) {
	std::vector<NodeStructs::MetaType> v;
	v.reserve(t.ors.size());
	for (const auto& e : t.ors) {
		auto exp = operator()(e);
		return_if_error(exp);
		v.push_back(std::move(exp).value());
	}
	return NodeStructs::MetaType{ NodeStructs::UnionType{ std::move(v) } };
}

R T::operator()(const NodeStructs::VariadicExpansionTypename& t) {
	throw;
}
