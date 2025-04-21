#include "../core/toCPP.hpp"
#include "../utility/vec_of_expected_to_expected_of_vec.hpp"
#include <iostream>

using T = traverse_type_visitor;
using R = T::R;

R T::operator()(const Realised::Type& type) {
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

R T::operator()(const Realised::PrimitiveType& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const Realised::FunctionType& t) {
	NOT_IMPLEMENTED;
	return std::nullopt;
}

R T::operator()(const Realised::InterfaceType& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const Realised::NamespaceType& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const Realised::Builtin& t) {
	return std::nullopt;
}

R T::operator()(const Realised::UnionType& t) {
	for (const auto& union_type : t.arguments)
		if (auto optional_error = operator()(union_type); optional_error.has_value())
			return optional_error.value();
	return std::nullopt;
}

R T::operator()(const Realised::TemplateType& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const Realised::EnumType& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const Realised::EnumValueType& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const Realised::OptionalType& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const Realised::AggregateType& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const Realised::VectorType& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const Realised::SetType& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const Realised::MapType& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const Realised::TypeListType& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const Realised::CompileTimeType& t) {
	NOT_IMPLEMENTED;
}
