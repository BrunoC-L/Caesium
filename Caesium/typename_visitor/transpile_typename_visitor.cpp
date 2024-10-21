#include "../core/toCPP.hpp"
#include <algorithm>

using T = transpile_typename_visitor;
using R = T::R;

R T::operator()(const NodeStructs::BaseTypename& type) {
#define OK(X) if (type.type == X) return X;
	OK("Int");
	OK("Bool");
	OK("String");
	OK("Char");
	OK("Void");
	OK("Floating");
#define OK(X) if (auto it = state.state.global_namespace. X .find(type.type); it != state.state.global_namespace. X .end()) return type.type;
	OK(types);
	OK(functions);
	OK(interfaces);
	OK(namespaces);
	OK(builtins);
	OK(blocks);
	OK(templates);
#undef OK

	if (auto it = state.state.global_namespace.aliases.find(type.type); it != state.state.global_namespace.aliases.end())
		return operator()(it->second);
	if (auto it = state.state.global_namespace.enums.find(type.type); it != state.state.global_namespace.enums.end())
		return "Int";
	return error{
		"user error",
		"undeclared identifier `" + type.type + "`"
	};
}

R T::operator()(const NodeStructs::NamespacedTypename& type) {
	auto type_of_namespace_or_e = type_of_typename(state, type.name_space.get());
	return_if_error(type_of_namespace_or_e);
	const auto& type_of_namespace = type_of_namespace_or_e.value();

	auto repr_of_typename_or_e = operator()(type.name_space.get());
	return_if_error(repr_of_typename_or_e);
	const auto& repr_of_typename = repr_of_typename_or_e.value();

	return std::visit(overload(
		[](const auto& e) -> R {
			throw;
		},
		[&](const NodeStructs::EnumType& e) -> R {
			const auto& _enum = e.enum_.get();
			if (auto it = std::find(_enum.values.begin(), _enum.values.end(), type.name_in_name_space); it != _enum.values.end())
				return _enum.name + "__" + type.name_in_name_space;
			throw;
		},
		[&](const NodeStructs::NamespaceType& e) -> R {
			if (auto it = e.name_space.get().types.find(type.name_in_name_space); it != e.name_space.get().types.end())
				return repr_of_typename + "__" + type.name_in_name_space;
			if (auto it = e.name_space.get().aliases.find(type.name_in_name_space); it != e.name_space.get().aliases.end())
				return operator()(it->second);
			if (auto it = e.name_space.get().namespaces.find(type.name_in_name_space); it != e.name_space.get().namespaces.end())
				return repr_of_typename + "__" + type.name_in_name_space;
			if (auto it = e.name_space.get().templates.find(type.name_in_name_space); it != e.name_space.get().templates.end())
				return repr_of_typename + "__" + type.name_in_name_space;
			throw;
		}
	), type_of_namespace.type._value);
}

R T::operator()(const NodeStructs::TemplatedTypename& type) {
	bool is_variant = cmp(type.type.get().value, NodeStructs::Typename::Variant_{ NodeStructs::BaseTypename{ "Union" } }) == std::weak_ordering::equivalent;
	bool is_vec_or_set =
		cmp(type.type.get().value, NodeStructs::Typename::Variant_{ NodeStructs::BaseTypename{ "Vector" } }) == std::weak_ordering::equivalent
		|| cmp(type.type.get().value, NodeStructs::Typename::Variant_{ NodeStructs::BaseTypename{ "Set" } }) == std::weak_ordering::equivalent;
	bool is_map = cmp(type.type.get().value, NodeStructs::Typename::Variant_{ NodeStructs::BaseTypename{ "Map" } }) == std::weak_ordering::equivalent;
	if (is_vec_or_set || is_map || is_variant) {
		if (is_vec_or_set && type.templated_with.size() != 1)
			throw;
		if (is_map && type.templated_with.size() != 2)
			throw;
		std::stringstream ss;
		auto tmpl = type_template_of_typename(state, type.templated_with, type.type);
		return_if_error(tmpl);
		auto t = operator()(type.type);
		return_if_error(t);
		ss << t.value() << "<";
		bool first = true;
		for (const auto& t : type.templated_with) {
			if (first)
				first = false;
			else
				ss << ", ";
			auto res = operator()(t);
			return_if_error(res);
			ss << res.value();
		}
		ss << ">";
		return ss.str();
	}
	else {
		std::stringstream ss;
		auto tmpl = type_template_of_typename(state, type.templated_with, type.type);
		return_if_error(tmpl);
		auto t = operator()(type.type);
		return_if_error(t);
		ss << t.value() << "__";
		bool first = true;
		for (const auto& t : type.templated_with) {
			if (first)
				first = false;
			else
				ss << "__";
			auto res = operator()(t);
			return_if_error(res);
			ss << res.value();
		}
		return ss.str();
	}
}

template<typename T>
void bubble_sort_swap(std::vector<T>& arr) {
	int n = arr.size();
	for (int i = 0; i < n - 1; ++i)
		for (int j = 0; j < n - i - 1; ++j)
			if (arr.at(j) > arr.at(j + 1))
				swap(arr.at(j), arr.at(j + 1));
}

void swap(NodeStructs::Typename& a, NodeStructs::Typename& b) {
	NodeStructs::Typename c = std::move(a);
	new (&a) NodeStructs::Typename(std::move(b));
	new (&b) NodeStructs::Typename(std::move(c));
}

R T::operator()(const NodeStructs::OptionalTypename& type) {
	auto inner_or_e = operator()(type.type);
	return_if_error(inner_or_e);
	return "Optional<" + std::move(inner_or_e).value() + ">";
}

R T::operator()(const NodeStructs::TupleTypename& type) {
	std::stringstream ss;
	ss << "Tuple<";
	auto ts = copy(type.members);
	if (ts.size() == 0)
		throw;
	bool first = true;
	for (const auto& t : ts) {
		if (first)
			first = false;
		else
			ss << ", ";
		auto res = operator()(t);
		return_if_error(res);
		ss << res.value();
	}
	ss << ">";
	return ss.str();
}

R T::operator()(const NodeStructs::UnionTypename& type) {
	std::stringstream ss;
	ss << "Union<";
	auto ts = copy(type.ors);
	if (ts.size() == 0)
		throw;
	bubble_sort_swap(ts); // todo unique
	bool first = true;
	for (const auto& t : ts) {
		if (first)
			first = false;
		else
			ss << ", ";
		auto res = operator()(t);
		return_if_error(res);
		ss << res.value();
	}
	ss << ">";
	return ss.str();
}

R T::operator()(const NodeStructs::VariadicExpansionTypename& t) {
	throw;
}
