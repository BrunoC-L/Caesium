#include "../core/toCPP.hpp"
#include "../utility/vec_of_expected_to_expected_of_vec.hpp"
#include <iostream>

using T = realise_typename_visitor;
using R = T::R;

R T::operator()(const NodeStructs::BaseTypename& type) {
#define case(language_keyword)\
	if (type.type == language_keyword##_tn_base.type)\
		return std::nullopt;
	expand_language_typenames(case)
#undef case
	auto it = find_by_name(state.state.global_namespace.aliases, type.type);
	if (it != state.state.global_namespace.aliases.end())
		return operator()(it->aliasTo);

	if (auto it = find_by_name(state.state.types_traversal.traversed, type.type); it != state.state.types_traversal.traversed.end())
		return std::nullopt;
	if (find_by_name(state.state.types_traversal.traversing, type.type))
		NOT_IMPLEMENTED_BUT_PROBABLY_ERROR;

	if (auto it = find_by_name(state.state.global_namespace.types, type.type); it != state.state.global_namespace.types.end()) {
		auto t_or_e = realise_type_or_interface(state, *it);
		return_if_error(t_or_e);
		if (t_or_e.value().name._value != type.type)
			NOT_IMPLEMENTED_BUT_PROBABLY_ERROR;
		if (auto it = find_by_name(state.state.types_traversal.traversed, type.type); it != state.state.types_traversal.traversed.end())
			return std::nullopt;
		else
			NOT_IMPLEMENTED_BUT_PROBABLY_ERROR;
	}
	if (auto it = find_by_name(state.state.global_namespace.enums, type.type); it != state.state.global_namespace.enums.end()) {
		return std::nullopt;
	}
	NOT_IMPLEMENTED;

	//if (opt_e.has_error()) {
	//	state.state.types_traversal.traversed.erase(key);
	//	return std::move(opt_e).error();
	//}

	//state.state.types_to_transpile.push_back(std::move(rt));
	//return std::nullopt;
}

R T::operator()(const NodeStructs::UnionTypename& t) {
	for (const auto& e : t.ors)
		if (auto err = operator()(e))
			return err;
	return std::nullopt;
}

R T::operator()(const NodeStructs::NamespacedTypename& t) {
	auto type_of_namespace = type_of_typename(state, variables, t.name_space);
	return_if_error(type_of_namespace);
	return caesium_lib::variant::visit(type_of_namespace.value().type.get(), overload(
		[&](const auto&) -> R {
			NOT_IMPLEMENTED;
		},
		[&](const Realised::EnumType& e) -> R {
			if (!state.state.enums_to_transpile.contains(e.name._value))
				NOT_IMPLEMENTED;// state.state.enums_to_transpile.insert({ e.name._value, copy(e.enum_.get()) });
			return std::nullopt;
		},
		[&](const Realised::NamespaceType&) -> R {
			NOT_IMPLEMENTED;
		}
	));
}

R T::operator()(const NodeStructs::OptionalTypename& t) {
	return operator()(t.type);
}

R T::operator()(const NodeStructs::TemplatedTypename& t) {
	auto type_of_template = type_of_typename(state, variables, t.type);
	return_if_error(type_of_template);
	return caesium_lib::variant::visit(type_of_template.value().type.get(), overload(
		[&](const auto&) -> R {
			NOT_IMPLEMENTED;
		},
		[&](const Realised::TemplateType&) -> R {
			for (const auto& e : t.templated_with) {
				if (holds<NodeStructs::Typename>(e.value))
					if (std::optional<error> opt_e = operator()(get<NodeStructs::Typename>(e.value)); opt_e.has_value())
						return opt_e;
				if (holds<std::string>(e.value)) {
					if (std::optional<error> opt_e = operator()(NodeStructs::BaseTypename{ get<std::string>(e.value) }); opt_e.has_value())
						NOT_IMPLEMENTED; // basically the error could be anything because maybe
				}
			}
			return std::nullopt;
		}
	));
}

R T::operator()(const NodeStructs::VariadicExpansionTypename& t) {
	return operator()(t.type);
}
