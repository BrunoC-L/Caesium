#include "../core/toCPP.hpp"
#include <algorithm>

using T = transpile_typename_visitor;
using R = T::R;

R T::operator()(const NodeStructs::BaseTypename& type) {
	if (auto it = state.state.named.type_aliases_typenames.find(type.type); it != state.state.named.type_aliases_typenames.end())
		return operator()(it->second);
	return type.type;
}

R T::operator()(const NodeStructs::NamespacedTypename& type) {
	return operator()(type.name_space.get()).value() + "__" + type.name_in_name_space;
}

R T::operator()(const NodeStructs::TemplatedTypename& type) {
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

R T::operator()(const NodeStructs::UnionTypename& type) {
	std::stringstream ss;
	ss << "Variant<";
	auto ts = type.ors;
	std::sort(ts.begin(), ts.end());
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
