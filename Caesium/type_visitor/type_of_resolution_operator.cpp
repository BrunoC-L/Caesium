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

R T::operator()(const Realised::Interface& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const Realised::NamespaceType& nst) {
	const auto& ns = nst.name_space.get();
	if (auto it = find_by_name(ns.types, accessed); it != ns.types.end()) {
		const auto& type = *it;
		auto e_or_t = realise_type_or_interface(state, type);
		return_if_error(e_or_t);
		return Realised::MetaType{ std::move(e_or_t).value() };
	}
	if (auto it = find_by_name(ns.aliases, accessed); it != ns.aliases.end()) {
		if (std::optional<error> err = realise_typename(state, variables, it->aliasTo); err.has_value())
			return err.value();
		auto e_t = type_of_typename(state, variables, it->aliasTo);
		return_if_error(e_t);
		return std::move(e_t).value();
	}
	if (auto it = find_by_name(ns.interfaces, accessed); it != ns.interfaces.end()) {
		NOT_IMPLEMENTED;
		/*const auto& interface = *it;
		if (!state.state.interfaces_traversal.traversed.contains(interface)) {
			state.state.interfaces_traversal.traversed.insert(copy(interface));
			state.state.interfaces_to_transpile.insert(copy(interface));
		}
		return Realised::MetaType{ Realised::Interface{ interface } };*/
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
	for (const auto& val : e.enum_.get().values)
		if (val == accessed) {
			auto full_name = e.enum_.get().name + "__" + accessed;
			if (auto it = find_by_name(state.state.global_namespace.aliases, full_name); it == state.state.global_namespace.aliases.end())
				state.state.global_namespace.aliases.push_back(NodeStructs::Alias{
					.name = full_name,
					.aliasTo = {
						.value = NodeStructs::NamespacedTypename{
							.name_space = {
								.value = NodeStructs::BaseTypename{ e.enum_.get().name },
								.category = std::nullopt,
								.info = rule_info_stub_no_throw()
							},
							.name_in_name_space = accessed
						},
						.category = std::nullopt,
						.info = rule_info_stub_no_throw()
					},
					.name_space = std::nullopt
				});
			return Realised::MetaType{ Realised::EnumValueType{
				.value_name = accessed,
				.full_name = full_name,
				.enum_ = copy(e),
			} };
		}
	return error{ "user error" , "Missing enum value `" + accessed + "` in enum `" + e.enum_.get().name + "`" };
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

R T::operator()(const Realised::TemplateInstanceType& t) {
	NOT_IMPLEMENTED;
}
