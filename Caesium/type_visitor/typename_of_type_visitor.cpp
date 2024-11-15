#include "../core/toCPP.hpp"
#include "../utility/vec_of_expected_to_expected_of_vec.hpp"

using T = typename_of_type_visitor;
using R = T::R;

R T::operator()(const NodeStructs::Type& t) {
	if (t.name_space.has_value())
		return NodeStructs::Expression{ NodeStructs::NamespacedTypename{ copy(t.name_space.value()), t.name}, NodeStructs::Value{} };
	else
		return NodeStructs::Expression{ NodeStructs::BaseTypename{ t.name }, NodeStructs::Value{} };
}

R T::operator()(const NodeStructs::PrimitiveType& t) {
	return typename_of_primitive(t);
}

R T::operator()(const NodeStructs::FunctionType& t) {
	return NodeStructs::Expression{ NodeStructs::BaseTypename{ t.name }, NodeStructs::Value{} };
}

R T::operator()(const NodeStructs::InterfaceType& t) {
	return NodeStructs::Expression{ NodeStructs::BaseTypename{ t.interface.get().name }, NodeStructs::Value{} };
}

R T::operator()(const NodeStructs::NamespaceType& t) {
	return NodeStructs::Expression{ NodeStructs::BaseTypename{ t.name_space.get().name }, NodeStructs::Value{} };
}

R T::operator()(const NodeStructs::Builtin& t) {
	return NodeStructs::Expression{ NodeStructs::BaseTypename{ t.name }, NodeStructs::Value{} };
}

R T::operator()(const NodeStructs::TupleType& t) {
	expected<std::vector<NodeStructs::Expression>> vec = vec_of_expected_to_expected_of_vec(t.arguments
		| std::views::transform([&](auto&& e) { return operator()(e); })
		| to_vec()
	);
	return_if_error(vec);
	return NodeStructs::Expression{ NodeStructs::TupleTypename{
		std::move(vec).value()
	}, NodeStructs::Value{} };
}

R T::operator()(const NodeStructs::UnionType& t) {
	expected<std::vector<NodeStructs::Expression>> vec = vec_of_expected_to_expected_of_vec(t.arguments
		| std::views::transform([&](auto&& e) { return operator()(e); })
		| to_vec()
	);
	return_if_error(vec);
	return NodeStructs::Expression{ NodeStructs::UnionTypename{
		std::move(vec).value()
	}, NodeStructs::Value{} };
}

R T::operator()(const NodeStructs::TemplateType& t) {
	if (t.name_space.get().name != "")
		throw; // return NodeStructs::Expression{ NodeStructs::NamespacedTypename{ copy(t.name_space.value()), t.name} };
	else
		return NodeStructs::Expression{ NodeStructs::BaseTypename{ t.name }, NodeStructs::Value{} };
}

R T::operator()(const NodeStructs::EnumType& t) {
	return NodeStructs::Expression{ NodeStructs::BaseTypename{ t.enum_.get().name }, NodeStructs::Value{} };
}

R T::operator()(const NodeStructs::EnumValueType& t) {
	return NodeStructs::Expression{
		NodeStructs::NamespacedTypename{
			NodeStructs::Expression{ NodeStructs::BaseTypename{ t.enum_.get().name }, NodeStructs::Value{} },
			t.value
	}, NodeStructs::Value{} };
}

R T::operator()(const NodeStructs::OptionalType& t) {
	auto tn_or_e = operator()(t.value_type);
	return_if_error(tn_or_e);
	return NodeStructs::Expression{ NodeStructs::OptionalTypename{ std::move(tn_or_e).value() }, NodeStructs::Value{} };
}

R T::operator()(const NodeStructs::AggregateType& t) {
	throw;
}

R T::operator()(const NodeStructs::Vector& t) {
	throw;
}

R T::operator()(const NodeStructs::VectorType& t) {
	std::vector<NodeStructs::Expression> v;
	{
		auto inner = operator()(t.value_type.get());
		return_if_error(inner);
		v.push_back(std::move(inner).value());
	}
	return NodeStructs::Expression{ NodeStructs::TemplatedTypename{
		.type = NodeStructs::Expression{ NodeStructs::BaseTypename{ "Vector" }, NodeStructs::Value{} },
		.templated_with = std::move(v)
	}, NodeStructs::Value{} };
}

R T::operator()(const NodeStructs::Set& t) {
	throw;
}

R T::operator()(const NodeStructs::SetType& t) {
	std::vector<NodeStructs::Expression> v;
	{
		auto inner = operator()(t.value_type.get());
		return_if_error(inner);
		v.push_back(std::move(inner).value());
	}
	return NodeStructs::Expression{ NodeStructs::TemplatedTypename{
		.type = NodeStructs::Expression{ NodeStructs::BaseTypename{ "Set" }, NodeStructs::Value{} },
		.templated_with = std::move(v)
	}, NodeStructs::Value{} };
}

R T::operator()(const NodeStructs::Map& t) {
	throw;
}

R T::operator()(const NodeStructs::MapType& t) {
	std::vector<NodeStructs::Expression> v;
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
	return NodeStructs::Expression{ NodeStructs::TemplatedTypename{
		.type = NodeStructs::Expression{ NodeStructs::BaseTypename{ "Map" }, NodeStructs::Value{} },
		.templated_with = std::move(v)
	}, NodeStructs::Value{} };
}

R T::operator()(const NodeStructs::CompileTimeType& t) {
	throw;
}
