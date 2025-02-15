#include "../core/toCPP.hpp"
#include "../utility/vec_of_expected_to_expected_of_vec.hpp"
#include <iostream>

using T = traverse_type_visitor;
using R = T::R;

R T::operator()(const NodeStructs::Type& type) {
	if (!state.state.traversed_types.contains(type)) {
		state.state.traversed_types.insert(copy(type));
		auto opt_e = transpile_type(state.unindented(), type);
		if (opt_e.has_error()) {
			state.state.traversed_types.erase(type);
			return std::move(opt_e).error();
		}
		state.state.traversed_types.insert(copy(type));
		state.state.types_to_transpile.push_back(copy(type));
	}
	return std::nullopt;
}

R T::operator()(const NodeStructs::PrimitiveType& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const NodeStructs::FunctionType& t) {
	NOT_IMPLEMENTED;
	return std::nullopt;
}

R T::operator()(const NodeStructs::InterfaceType& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const NodeStructs::NamespaceType& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const NodeStructs::Builtin& t) {
	return std::nullopt;
}

R T::operator()(const NodeStructs::UnionType& t) {
	for (const auto& union_type : t.arguments)
		if (auto optional_error = operator()(union_type); optional_error.has_value())
			return optional_error.value();
	return std::nullopt;
}

R T::operator()(const NodeStructs::TemplateType& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const NodeStructs::EnumType& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const NodeStructs::EnumValueType& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const NodeStructs::OptionalType& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const NodeStructs::AggregateType& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const NodeStructs::Vector& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const NodeStructs::VectorType& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const NodeStructs::Set& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const NodeStructs::SetType& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const NodeStructs::Map& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const NodeStructs::MapType& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const NodeStructs::TypeList& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const NodeStructs::TypeListType& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const NodeStructs::TypeToken& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const NodeStructs::CompileTimeType& t) {
	NOT_IMPLEMENTED;
}
