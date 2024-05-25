#include "../core/toCPP.hpp"

using T = typename_original_representation_visitor;
using R = T::R;

R T::operator()(const NodeStructs::BaseTypename& type) {
	return type.type;
}

R T::operator()(const NodeStructs::NamespacedTypename& type) {
	return operator()(type.name_space) + type.name_in_name_space;
}

R T::operator()(const NodeStructs::TemplatedTypename& type) {
	std::stringstream ss;
	ss << operator()(type.type.get()) << "<";
	for (const auto& tn : type.templated_with)
		ss << operator()(tn) << ", ";
	ss << ">";
	return ss.str();
}

R T::operator()(const NodeStructs::UnionTypename& type) {
	std::stringstream ss;
	bool has_prev = false;
	for (const auto& tn : type.ors) {
		if (has_prev)
			ss << "|";
		else
			has_prev = true;
		ss << operator()(tn);
	}
	return ss.str();
}
