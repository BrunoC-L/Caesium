#include "../core/toCPP.hpp"
#include "../utility/vec_of_expected_to_expected_of_vec.hpp"

using T = typename_of_type_visitor;
using R = T::R;

R T::operator()(const NodeStructs::Type& t) {
	if (t.name_space.has_value())
		return make_typename(NodeStructs::NamespacedTypename{ copy(t.name_space.value()), t.name}, NodeStructs::Value{}, rule_info_stub_no_throw());
	else
		return make_typename(NodeStructs::BaseTypename{ t.name }, NodeStructs::Value{}, rule_info_stub_no_throw());
}

R T::operator()(const NodeStructs::PrimitiveType& t) {
	return typename_of_primitive(t);
}

R T::operator()(const NodeStructs::FunctionType& t) {
	return make_typename(NodeStructs::BaseTypename{ t.name }, NodeStructs::Value{}, rule_info_stub_no_throw());
}

R T::operator()(const NodeStructs::InterfaceType& t) {
	return make_typename(NodeStructs::BaseTypename{ t.interface.get().name }, NodeStructs::Value{}, rule_info_stub_no_throw());
}

R T::operator()(const NodeStructs::NamespaceType& t) {
	return make_typename(NodeStructs::BaseTypename{ t.name_space.get().name }, NodeStructs::Value{}, rule_info_stub_no_throw());
}

R T::operator()(const NodeStructs::Builtin& t) {
	return make_typename(NodeStructs::BaseTypename{ t.name }, NodeStructs::Value{}, rule_info_stub_no_throw());
}

R T::operator()(const NodeStructs::UnionType& t) {
	expected<std::vector<NodeStructs::Typename>> vec = vec_of_expected_to_expected_of_vec(t.arguments
		| std::views::transform([&](auto&& e) { return operator()(e); })
		| to_vec()
	);
	return_if_error(vec);
	return make_typename(NodeStructs::UnionTypename{
		std::move(vec).value()
		}, NodeStructs::Value{}, rule_info_stub_no_throw());
}

R T::operator()(const NodeStructs::TemplateType& t) {
	if (t.name_space.get().name != "")
		throw; // return make_typename(NodeStructs::NamespacedTypename{ copy(t.name_space.value()), t.name});
	else
		return make_typename(NodeStructs::BaseTypename{ t.name }, NodeStructs::Value{}, rule_info_stub_no_throw());
}

R T::operator()(const NodeStructs::EnumType& t) {
	return make_typename(NodeStructs::BaseTypename{ t.enum_.get().name }, NodeStructs::Value{}, rule_info_stub_no_throw());
}

R T::operator()(const NodeStructs::EnumValueType& t) {
	return make_typename(
		NodeStructs::NamespacedTypename{
			make_typename(NodeStructs::BaseTypename{ t.enum_.get().name }, NodeStructs::Value{}, rule_info_stub_no_throw()),
			t.value
		}, NodeStructs::Value{}, rule_info_stub_no_throw());
}

R T::operator()(const NodeStructs::OptionalType& t) {
	auto tn_or_e = operator()(t.value_type);
	return_if_error(tn_or_e);
	return make_typename(NodeStructs::OptionalTypename{ std::move(tn_or_e).value() }, NodeStructs::Value{}, rule_info_stub_no_throw());
}

R T::operator()(const NodeStructs::AggregateType& t) {
	throw;
}

R T::operator()(const NodeStructs::Vector& t) {
	throw;
}

R T::operator()(const NodeStructs::VectorType& t) {
	std::vector<NodeStructs::WordTypenameOrExpression> v;
	{
		auto inner = operator()(t.value_type);
		return_if_error(inner);
		v.push_back(NodeStructs::WordTypenameOrExpression{ std::move(inner).value() });
	}
	return make_typename(NodeStructs::TemplatedTypename{
		.type = make_typename(NodeStructs::BaseTypename{ "Vector" }, NodeStructs::Value{}, rule_info_stub_no_throw()),
		.templated_with = std::move(v)
		}, NodeStructs::Value{}, rule_info_stub_no_throw());
}

R T::operator()(const NodeStructs::Set& t) {
	throw;
}

R T::operator()(const NodeStructs::SetType& t) {
	std::vector<NodeStructs::WordTypenameOrExpression> v;
	{
		auto inner = operator()(t.value_type);
		return_if_error(inner);
		v.push_back(NodeStructs::WordTypenameOrExpression{ std::move(inner).value() });
	}
	return make_typename(NodeStructs::TemplatedTypename{
		.type = make_typename(NodeStructs::BaseTypename{ "Set" }, NodeStructs::Value{}, rule_info_stub_no_throw()),
		.templated_with = std::move(v)
		}, NodeStructs::Value{}, rule_info_stub_no_throw());
}

R T::operator()(const NodeStructs::Map& t) {
	throw;
}

R T::operator()(const NodeStructs::MapType& t) {
	std::vector<NodeStructs::WordTypenameOrExpression> v;
	{
		auto inner = operator()(t.key_type);
		return_if_error(inner);
		v.push_back(NodeStructs::WordTypenameOrExpression{ std::move(inner).value() });
	}
	{
		auto inner = operator()(t.value_type);
		return_if_error(inner);
		v.push_back(NodeStructs::WordTypenameOrExpression{ std::move(inner).value() });
	}
	return make_typename(NodeStructs::TemplatedTypename{
		.type = make_typename(NodeStructs::BaseTypename{ "Map" }, NodeStructs::Value{}, rule_info_stub_no_throw()),
		.templated_with = std::move(v)
		}, NodeStructs::Value{}, rule_info_stub_no_throw());
}

R T::operator()(const NodeStructs::TypeList& t) {
	throw;
}

R T::operator()(const NodeStructs::TypeListType& t) {
	throw;
}

R T::operator()(const NodeStructs::TypeToken& t) {
	throw;
}

R T::operator()(const NodeStructs::CompileTimeType& t) {
	throw;
}
