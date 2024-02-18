#include <algorithm>
#include "transpile_typename_visitor.hpp"
#include "type_template_of_typename_visitor.hpp"

using T = transpile_typename_visitor;
using R = T::R;

R T::operator()(const NodeStructs::BaseTypename& type) {
	if (auto it = state.state.named.type_aliases_typenames.find(type.type); it != state.state.named.type_aliases_typenames.end())
		return operator()(it->second);
	return type.type;
}

R T::operator()(const NodeStructs::NamespacedTypename& type) {
	return operator()(type.name_space.get()).value() + "::" + operator()(type.name_in_name_space.get()).value();
}

R T::operator()(const NodeStructs::TemplatedTypename& type) {
	std::stringstream ss;
	auto tmpl = type_template_of_typename_visitor{ {}, state, type.templated_with }(type.type.get());
	return_if_error(tmpl);
	auto t = operator()(type.type.get());
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
