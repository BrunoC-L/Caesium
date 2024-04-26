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
	if (!std::holds_alternative<NodeStructs::BaseTypename>(type.name_space.get().value._value))
		throw;
	const std::string& ns_str = std::get<NodeStructs::BaseTypename>(type.name_space.get().value._value).type;
	if (auto it = state.state.global_namespace.namespaces.find(ns_str); it != state.state.global_namespace.namespaces.end()) {
		if (auto it2 = it->second.types.find(type.name_in_name_space); it2 != it->second.types.end())
			return operator()(type.name_space.get()).value() + "__" + type.name_in_name_space;
		if (auto it2 = it->second.aliases.find(type.name_in_name_space); it2 != it->second.aliases.end())
			return operator()(it2->second);
		throw;
	}
	else if (auto it = state.state.global_namespace.enums.find(ns_str); it != state.state.global_namespace.enums.end()) {
		if (it->second.size() != 1)
			throw;
		const auto& enum_ = it->second.back();
		if (auto it = std::find(enum_.values.begin(), enum_.values.end(), type.name_in_name_space); it != enum_.values.end())
			return ns_str + "__" + type.name_in_name_space;
		throw;
	}
	throw;
}

R T::operator()(const NodeStructs::TemplatedTypename& type) {
	bool is_variant = type.type.get() <=> NodeStructs::Typename{ NodeStructs::BaseTypename{ "Variant" } } == std::weak_ordering::equivalent;
	bool is_vec_or_set =
		type.type.get() <=> NodeStructs::Typename{ NodeStructs::BaseTypename{ "Vector" } } == std::weak_ordering::equivalent
		|| type.type.get() <=> NodeStructs::Typename{ NodeStructs::BaseTypename{ "Set" } } == std::weak_ordering::equivalent;
	bool is_map = type.type.get() <=> NodeStructs::Typename{ NodeStructs::BaseTypename{ "Map" } } == std::weak_ordering::equivalent;
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

R T::operator()(const NodeStructs::UnionTypename& type) {
	std::stringstream ss;
	ss << "Variant<";
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
