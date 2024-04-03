#include "../core/toCPP.hpp"
#include "../utility/vec_of_expected_to_expected_of_vec.hpp"

using T = traverse_type_visitor;
using R = T::R;

R T::operator()(const std::reference_wrapper<const NodeStructs::Type>& type) {
	if (!state.state.traversed_types.contains(type.get())) {
		state.state.traversed_types.insert(type);
		state.state.types_to_transpile.insert(type);
	}
	return std::nullopt;
}

R T::operator()(const NodeStructs::PrimitiveType& t) {
	throw;
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

R T::operator()(const NodeStructs::Builtin& t) {
	throw;
}

R T::operator()(const NodeStructs::UnionType& t) {
	for (const auto& union_type : t.arguments)
		if (auto optional_error = operator()(union_type); optional_error.has_value())
			return optional_error.value();
	return std::nullopt;
}

R T::operator()(const NodeStructs::TemplateType& t) {
	throw;
}

R T::operator()(const NodeStructs::Enum& t) {
	throw;
}

R T::operator()(const NodeStructs::Vector& t) {
	throw;
}

R T::operator()(const NodeStructs::VectorType& t) {
	throw;
}

R T::operator()(const NodeStructs::Set& t) {
	throw;
}

R T::operator()(const NodeStructs::SetType& t) {
	throw;
}

R T::operator()(const NodeStructs::Map& t) {
	throw;
}

R T::operator()(const NodeStructs::MapType& t) {
	throw;
}
