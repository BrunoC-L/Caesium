#include "../core/toCPP.hpp"

using T = type_of_typename_visitor;
using R = T::R;

R T::operator()(const NodeStructs::BaseTypename& t) {
	if (t.type == "Int")
		return Realised::MetaType{ Realised::PrimitiveType{ Realised::PrimitiveType::NonValued<int>{} } };
	if (t.type == "Bool")
		return Realised::MetaType{ Realised::PrimitiveType{ Realised::PrimitiveType::NonValued<bool>{} } };
	if (t.type == "String")
		return Realised::MetaType{ Realised::PrimitiveType{ Realised::PrimitiveType::NonValued<std::string>{} } };
	if (t.type == "Char")
		return Realised::MetaType{ Realised::PrimitiveType{ Realised::PrimitiveType::NonValued<char>{} } };
	if (t.type == "Void")
		return Realised::MetaType{ Realised::PrimitiveType{ Realised::PrimitiveType::NonValued<Realised::void_t>{} } };
	if (t.type == "Floating")
		return Realised::MetaType{ Realised::PrimitiveType{ Realised::PrimitiveType::NonValued<double>{} } };
	if (t.type == "None") {
		NOT_IMPLEMENTED;
		return Realised::MetaType{ Realised::PrimitiveType{ Realised::PrimitiveType::NonValued<Realised::empty_optional_t>{} } };
	}

	if (auto it = state.state.global_namespace.types.find(t.type); it != state.state.global_namespace.types.end()) {
		const auto& types = it->second;
		if (types.size() != 1)
			NOT_IMPLEMENTED;
		const auto& type = types.at(0);
		NOT_IMPLEMENTED;
		//auto opt_e = traverse_type(state, type);
		//if (opt_e.has_value())
		//	return opt_e.value();
		//// fall-through to next
	}

	if (auto it = state.state.types_traversal.traversed.find(t.type); it != state.state.types_traversal.traversed.end()) {
		NOT_IMPLEMENTED;
		/*for (const auto& t_ : state.state.types_to_transpile)
			if (t_.name == t.type)
				return Realised::MetaType{ copy(t_) };*/
	}

	bool has_f = state.state.global_namespace.functions.find(t.type) != state.state.global_namespace.functions.end();
	if (has_f) {
		NOT_IMPLEMENTED;
		/*return Realised::MetaType{ Realised::FunctionType{
			t.type, state.state.global_namespace
		} };*/
	}
	if (auto it = state.state.global_namespace.templates.find(t.type); it != state.state.global_namespace.templates.end()) {
		NOT_IMPLEMENTED;
		//return Realised::MetaType{ Realised::TemplateType{ t.type, state.state.global_namespace } };
	}
	if (auto it = state.state.global_namespace.aliases.find(t.type); it != state.state.global_namespace.aliases.end()) {
		const auto& type_name = it->second;
		auto type = type_of_typename(state, variables, type_name);
		return_if_error(type);
		if (std::optional<error> err = traverse_type(state, type.value()); err.has_value())
			return err.value();
		return std::move(type).value();
	}
	if (auto it = state.state.global_namespace.interfaces.find(t.type); it != state.state.global_namespace.interfaces.end()) {
		NOT_IMPLEMENTED;
		/*const auto& interfaces = it->second;
		if (interfaces.size() != 1)
			NOT_IMPLEMENTED;
		const auto& interface = interfaces.at(0);
		if (!state.state.interfaces_traversal.traversed.contains(interface)) {
			state.state.interfaces_traversal.traversed.insert(copy(interface));
			state.state.interfaces_to_transpile.insert(copy(interface));
		}
		return Realised::MetaType{ NodeStructs::InterfaceType{ interface } };*/
	}
	if (auto it = state.state.global_namespace.namespaces.find(t.type); it != state.state.global_namespace.namespaces.end())
		return Realised::MetaType{ Realised::NamespaceType{ it->first, it->second } };
	NOT_IMPLEMENTED;
	/*if (auto it = state.state.global_namespace.builtins.find(t.type); it != state.state.global_namespace.builtins.end())
		return Realised::MetaType{ Realised::Builtin{ t.type } };*/
	if (auto it = state.state.global_namespace.enums.find(t.type); it != state.state.global_namespace.enums.end()) {
		NOT_IMPLEMENTED;
		//const std::vector<NodeStructs::Enum>& enums = it->second;
		//if (enums.size() != 1)
		//	NOT_IMPLEMENTED;
		//const NodeStructs::Enum& enum_ = enums.at(0);
		//if (!state.state.enums_to_transpile.contains(enum_))
		//	state.state.enums_to_transpile.insert(copy(enum_));
		//NOT_IMPLEMENTED;
		////return Realised::MetaType{ NodeStructs::EnumType{ enum_ } };
	}
	return error{ "user error" , "Missing type `" + t.type + "`"};
}

R T::operator()(const NodeStructs::NamespacedTypename& t) {
	auto e_space_t = operator()(t.name_space);
	return_if_error(e_space_t);
	const auto& space_t = e_space_t.value();
	return type_of_resolution_operator(state, variables, space_t, t.name_in_name_space);
}

R T::operator()(const NodeStructs::TemplatedTypename& tt) {
	std::vector<NodeStructs::WordTypenameOrExpression> templated_with;
	for (const auto& t : tt.templated_with) {
		auto arg = type_of_typename(state, variables, t);
		return_if_error(arg);
		auto tn = typename_of_type(state, arg.value());
		return_if_error(tn);
		templated_with.push_back({ std::move(tn).value() });
	}
	return type_template_of_typename(state, variables, templated_with, tt.type);
}

R T::operator()(const NodeStructs::OptionalTypename& t) {
	auto t_or_e = operator()(t.type);
	return_if_error(t_or_e);
	NOT_IMPLEMENTED;
	//return Realised::MetaType{ Realised::OptionalType{ std::move(t_or_e).value() } };
}

R T::operator()(const NodeStructs::UnionTypename& t) {
	std::vector<Realised::MetaType> v;
	v.reserve(t.ors.size());
	for (const auto& e : t.ors) {
		auto exp = operator()(e);
		return_if_error(exp);
		v.push_back(std::move(exp).value());
	}
	NOT_IMPLEMENTED;
	//return Realised::MetaType{ Realised::UnionType{ std::move(v) } };
}

R T::operator()(const NodeStructs::VariadicExpansionTypename& t) {
	NOT_IMPLEMENTED;
}
