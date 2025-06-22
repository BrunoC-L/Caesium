#include "../core/toCPP.hpp"

using T = type_of_typename_visitor;
using R = T::R;

R T::operator()(const NodeStructs::BaseTypename& t) {
#define case(language_keyword)\
	if (t.type == language_keyword##_tn_base.type)\
		return Realised::MetaType{ Realised::PrimitiveType{ Realised::PrimitiveType::language_keyword{} } };
	expand_language_typenames_that_can_appear_in_cpp(case)
#undef case

	// getting the type of auto wouldnt make sense
	if (t.type == auto_tn_base.type)
		NOT_IMPLEMENTED_BUT_PROBABLY_ERROR;

	if (auto it = find_by_name(state.state.global_namespace.types, t.type); it != state.state.global_namespace.types.end())
		return realise_type_or_interface(state, *it).transform([](Realised::Type t) { return Realised::MetaType{ std::move(t) }; });

	if (auto it = state.state.types_traversal.traversed.find(t.type); it != state.state.types_traversal.traversed.end())
		return Realised::MetaType{ copy(it->second) };

	if (auto f = find_by_name(state.state.global_namespace.functions, t.type); f != state.state.global_namespace.functions.end()) {
		NOT_IMPLEMENTED;
		/*return Realised::MetaType{ Realised::FunctionType{
			t.type, state.state.global_namespace
		} };*/
	}
	if (auto it = find_by_name(state.state.global_namespace.templates, t.type); it != state.state.global_namespace.templates.end()) {
		return Realised::MetaType{ Realised::TemplateType{ t.type } };
	}
	if (auto it = find_by_name(state.state.global_namespace.aliases, t.type); it != state.state.global_namespace.aliases.end()) {
		const auto& type_name = it->aliasTo;
		if (std::optional<error> err = realise_typename(state, variables, type_name); err.has_value())
			return err.value();
		auto type = type_of_typename(state, variables, type_name);
		return_if_error(type);
		return std::move(type).value();
	}
	if (auto it = find_by_name(state.state.global_namespace.interfaces, t.type); it != state.state.global_namespace.interfaces.end())
		return realise_type_or_interface(state, *it).transform([](Realised::Interface t) { return Realised::MetaType{ std::move(t) }; });
	if (auto it = find_by_name(state.state.global_namespace.namespaces, t.type); it != state.state.global_namespace.namespaces.end())
		return Realised::MetaType{ Realised::NamespaceType{ it->name, *it } };
	if (auto it = find_by_name(state.state.types, t.type); it != state.state.types.end())
		return copy(it->second);
	if (auto it = find_by_name(state.state.global_namespace.enums, t.type); it != state.state.global_namespace.enums.end()) {
		if (!state.state.enums_to_transpile.contains(t.type))
			state.state.enums_to_transpile.insert({ t.type, copy(*it) });
		return Realised::MetaType{ Realised::EnumType{ it->name, *it } };
	}
	if (auto it = find_by_name(state.state.enums_to_transpile, t.type); it != state.state.enums_to_transpile.end())
		NOT_IMPLEMENTED;
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
	templated_with.reserve(tt.templated_with.size());
	for (const auto& t : tt.templated_with) {
		auto arg = type_of_typename(state, variables, t);
		return_if_error(arg);
		auto tn = name_of_type(state, arg.value());
		return_if_error(tn);
		templated_with.push_back({ NodeStructs::Typename{
			NodeStructs::BaseTypename{ std::move(tn).value() },
			category_of_word_typename_or_expression(state, variables, t),
			info_of_word_typename_or_expression(state, variables, t)
		} });
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
	return Realised::MetaType{ Realised::UnionType{ "TODO??", std::move(v) } };
}

R T::operator()(const NodeStructs::VariadicExpansionTypename& t) {
	NOT_IMPLEMENTED;
}
