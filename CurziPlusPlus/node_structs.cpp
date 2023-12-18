#include "node_structs.h"

std::strong_ordering NodeStructs::UnionTypename::operator<=>(const UnionTypename& other) const {
	std::strong_ordering size_cmp = ors.size() <=> other.ors.size();
	if (size_cmp != 0)
		return size_cmp;
	for (size_t i = 0; i < ors.size(); ++i) {
		std::strong_ordering val_cmp = ors.at(i) <=> other.ors.at(i);
		if (val_cmp != 0)
			return val_cmp;
	}
	return std::strong_ordering::equal;
}

std::strong_ordering NodeStructs::TemplatedTypename::operator<=>(const TemplatedTypename& other) const {
	std::strong_ordering tmpl_cmp = type <=> other.type;
	if (tmpl_cmp != 0)
		return tmpl_cmp;

	std::strong_ordering size_cmp = templated_with.size() <=> other.templated_with.size();
	if (size_cmp != 0)
		return size_cmp;

	for (size_t i = 0; i < templated_with.size(); ++i) {
		std::strong_ordering val_cmp = templated_with.at(i) <=> other.templated_with.at(i);
		if (val_cmp != 0)
			return val_cmp;
	}

	return std::strong_ordering::equal;
}

std::strong_ordering NodeStructs::NamespacedTypename::operator<=>(const NamespacedTypename& other) const {
	std::strong_ordering ns_cmp = name_space <=> other.name_space;
	if (ns_cmp != 0)
		return ns_cmp;
	return name_in_name_space <=> other.name_in_name_space;
}
