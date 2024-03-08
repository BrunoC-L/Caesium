#include "../core/toCPP.hpp"
#include "../utility/vec_of_expected_to_expected_of_vec.hpp"

using T = traverse_type_visitor;
using R = T::R;

R T::operator()(const NodeStructs::Type& type) {
	if (!state.state.traversed_types.contains(type)) {
		state.state.traversed_types.insert(type);
		auto t = transpile(state, type);
		return_if_error(t);
		state.state.transpile_in_reverse_order.push_back(std::move(t).value());
	}
	return std::nullopt;
}

//R T::operator()(const NodeStructs::AggregateType& t) {
//	throw;
//	return std::nullopt;
//}

R T::operator()(const NodeStructs::TypeType& t) {
	throw;
	return std::nullopt;
}

R T::operator()(const NodeStructs::FunctionType& t) {
	throw;
	return std::nullopt;
}

R T::operator()(const NodeStructs::InterfaceType& t) {
	throw;
}

R T::operator()(const NodeStructs::NamespaceType& t) {
	throw;
}

R T::operator()(const NodeStructs::UnionType& t) {
	for (const auto& union_type : t.arguments)
		if (auto optional_error = operator()(union_type); optional_error.has_value())
			return optional_error.value();
	return std::nullopt;
}

R T::operator()(const NodeStructs::VectorType& t) {
	throw;
}

R T::operator()(const NodeStructs::SetType& t) {
	throw;
}

R T::operator()(const NodeStructs::MapType& t) {
	throw;
}

R T::operator()(const NodeStructs::Template& t) {
	throw;
}

R T::operator()(const NodeStructs::BuiltInType& t) {
	throw;
}

R T::operator()(const std::string&) {
	throw;
	return std::nullopt;
}

R T::operator()(const double&) {
	throw;
	return std::nullopt;
}

R T::operator()(const int&) {
	throw;
	return std::nullopt;
}

R T::operator()(const bool&) {
	throw;
}
