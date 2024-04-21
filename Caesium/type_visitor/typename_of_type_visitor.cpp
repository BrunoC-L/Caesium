#include "../core/toCPP.hpp"
#include "../utility/vec_of_expected_to_expected_of_vec.hpp"

using T = typename_of_type_visitor;
using R = T::R;

R T::operator()(const NodeStructs::Type& t) {
	if (t.name_space.has_value())
		return NodeStructs::Typename{ NodeStructs::NamespacedTypename{ copy(t.name_space.value()), t.name}};
	else
		return NodeStructs::Typename{ NodeStructs::BaseTypename{ t.name } };
}

R T::operator()(const NodeStructs::PrimitiveType& t) {
	return typename_of_primitive(t);
}

R T::operator()(const NodeStructs::FunctionType& t) {
	throw;
}

R T::operator()(const NodeStructs::InterfaceType& t) {
	throw;
}

R T::operator()(const NodeStructs::NamespaceType& t) {
	throw;
}

R T::operator()(const NodeStructs::Builtin& t) {
	return NodeStructs::Typename{ NodeStructs::BaseTypename{ t.name } };
}

R T::operator()(const NodeStructs::UnionType& t) {
	expected<std::vector<NodeStructs::Typename>> vec = vec_of_expected_to_expected_of_vec(t.arguments | LIFT_TRANSFORM(operator()) | to_vec());
	return_if_error(vec);
	return NodeStructs::Typename{ NodeStructs::UnionTypename{
		.ors = std::move(vec).value()
	} };
}

R T::operator()(const NodeStructs::TemplateType& t) {
	throw;
}

R T::operator()(const NodeStructs::EnumType& t) {
	throw;
}

R T::operator()(const NodeStructs::EnumValueType& t) {
	throw;
}

R T::operator()(const NodeStructs::AggregateType& t) {
	throw;
}

R T::operator()(const NodeStructs::Vector& t) {
	throw;
}

R T::operator()(const NodeStructs::VectorType& t) {
	auto inner = operator()(t.value_type.get());
	return_if_error(inner);
	std::vector<NodeStructs::Typename> v;
	v.push_back(std::move(inner).value());
	return NodeStructs::Typename{ NodeStructs::TemplatedTypename{
		.type = NodeStructs::Typename{ NodeStructs::BaseTypename{ "Vector" } },
		.templated_with = std::move(v)
	} };
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