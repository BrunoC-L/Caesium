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
	return NodeStructs::Typename{ NodeStructs::BaseTypename{ t.name } };
}

R T::operator()(const NodeStructs::InterfaceType& t) {
	return NodeStructs::Typename{ NodeStructs::BaseTypename{ t.interface.get().name } };
}

R T::operator()(const NodeStructs::NamespaceType& t) {
	return NodeStructs::Typename{ NodeStructs::BaseTypename{ t.name_space.get().name } };
}

R T::operator()(const NodeStructs::Builtin& t) {
	return NodeStructs::Typename{ NodeStructs::BaseTypename{ t.name } };
}

R T::operator()(const NodeStructs::TupleType& t) {
	expected<std::vector<NodeStructs::Typename>> vec = vec_of_expected_to_expected_of_vec(t.arguments
		| std::views::transform([&](auto&& e) { return operator()(e); })
		| to_vec()
	);
	return_if_error(vec);
	return NodeStructs::Typename{ NodeStructs::TupleTypename{
		std::move(vec).value()
	} };
}

R T::operator()(const NodeStructs::UnionType& t) {
	expected<std::vector<NodeStructs::Typename>> vec = vec_of_expected_to_expected_of_vec(t.arguments
		| std::views::transform([&](auto&& e) { return operator()(e); })
		| to_vec()
	);
	return_if_error(vec);
	return NodeStructs::Typename{ NodeStructs::UnionTypename{
		std::move(vec).value()
	} };
}

R T::operator()(const NodeStructs::TemplateType& t) {
	if (t.name_space.get().name != "")
		throw; // return NodeStructs::Typename{ NodeStructs::NamespacedTypename{ copy(t.name_space.value()), t.name} };
	else
		return NodeStructs::Typename{ NodeStructs::BaseTypename{ t.name } };
}

R T::operator()(const NodeStructs::EnumType& t) {
	return NodeStructs::Typename{ NodeStructs::BaseTypename{ t.enum_.get().name } };
}

R T::operator()(const NodeStructs::EnumValueType& t) {
	return NodeStructs::Typename{ NodeStructs::NamespacedTypename{ NodeStructs::Typename{ NodeStructs::BaseTypename{ t.enum_.get().name } }, t.value } };
}

R T::operator()(const NodeStructs::OptionalType& t) {
	auto tn_or_e = operator()(t.value_type);
	return_if_error(tn_or_e);
	return NodeStructs::Typename{ NodeStructs::OptionalTypename{ std::move(tn_or_e).value() } };
}

R T::operator()(const NodeStructs::AggregateType& t) {
	throw;
}

R T::operator()(const NodeStructs::Vector& t) {
	throw;
}

R T::operator()(const NodeStructs::VectorType& t) {
	std::vector<NodeStructs::Typename> v;
	{
		auto inner = operator()(t.value_type.get());
		return_if_error(inner);
		v.push_back(std::move(inner).value());
	}
	return NodeStructs::Typename{ NodeStructs::TemplatedTypename{
		.type = NodeStructs::Typename{ NodeStructs::BaseTypename{ "Vector" } },
		.templated_with = std::move(v)
	} };
}

R T::operator()(const NodeStructs::Set& t) {
	throw;
}

R T::operator()(const NodeStructs::SetType& t) {
	std::vector<NodeStructs::Typename> v;
	{
		auto inner = operator()(t.value_type.get());
		return_if_error(inner);
		v.push_back(std::move(inner).value());
	}
	return NodeStructs::Typename{ NodeStructs::TemplatedTypename{
		.type = NodeStructs::Typename{ NodeStructs::BaseTypename{ "Set" } },
		.templated_with = std::move(v)
	} };
}

R T::operator()(const NodeStructs::Map& t) {
	throw;
}

R T::operator()(const NodeStructs::MapType& t) {
	std::vector<NodeStructs::Typename> v;
	{
		auto inner = operator()(t.key_type.get());
		return_if_error(inner);
		v.push_back(std::move(inner).value());
	}
	{
		auto inner = operator()(t.value_type.get());
		return_if_error(inner);
		v.push_back(std::move(inner).value());
	}
	return NodeStructs::Typename{ NodeStructs::TemplatedTypename{
		.type = NodeStructs::Typename{ NodeStructs::BaseTypename{ "Map" } },
		.templated_with = std::move(v)
	} };
}
