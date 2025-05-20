#include "../core/toCPP.hpp"
#include "../utility/vec_of_expected_to_expected_of_vec.hpp"
#include <iostream>

using T = realise_typename_visitor;
using R = T::R;

R T::operator()(const NodeStructs::BaseTypename& type) {
	NOT_IMPLEMENTED;
	//Realised::RealisedTypeId key{ type.name, copy(type.name_space) };
	//if (!state.state.types_traversal.traversed.contains(key)) {

	//	// insert now in case it references itself and we recurse
	//	state.state.types_traversal.traversed.insert(copy(key));

	//	auto rt_or_e = realise(state, type);
	//	return_if_error(rt_or_e);
	//	Realised::Type rt = std::move(rt_or_e).value();
	//	auto opt_e = transpile_type(state.unindented(), rt);

	//	if (opt_e.has_error()) {
	//		state.state.types_traversal.traversed.erase(key);
	//		return std::move(opt_e).error();
	//	}

	//	state.state.types_to_transpile.push_back(std::move(rt));
	//}
	//return std::nullopt;
}

R T::operator()(const NodeStructs::UnionTypename& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const NodeStructs::NamespacedTypename& t) {
		NOT_IMPLEMENTED;
}

R T::operator()(const NodeStructs::OptionalTypename& t) {
		NOT_IMPLEMENTED;
}

R T::operator()(const NodeStructs::TemplatedTypename& t) {
		NOT_IMPLEMENTED;
}

R T::operator()(const NodeStructs::VariadicExpansionTypename& t) {
		NOT_IMPLEMENTED;
}
