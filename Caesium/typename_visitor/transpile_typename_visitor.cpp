#include "../core/toCPP.hpp"
#include <algorithm>

using T = transpile_typename_visitor;
using R = T::R;

R T::operator()(const NodeStructs::BaseTypename& type) {
	if (auto it = state.state.global_namespace.aliases.find(type.type); it != state.state.global_namespace.aliases.end())
		return operator()(it->second);
	return type.type;
}

R T::operator()(const NodeStructs::NamespacedTypename& type) {
	return operator()(type.name_space.get()).value() + "__" + type.name_in_name_space;
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
			ss << operator()(t).value();
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
			ss << operator()(t).value();
		}
		return ss.str();
	}
}

R T::operator()(const NodeStructs::UnionTypename& type) {
	std::stringstream ss;
	ss << "Variant<";
	auto ts = type.ors;
	if (ts.size() == 0)
		throw;
	std::sort(ts.begin(), ts.end()); // todo unique
	bool first = true;
	for (const auto& t : ts) {
		if (first)
			first = false;
		else
			ss << ", ";
		ss << operator()(t).value();
	}
	ss << ">";
	return ss.str();
}
