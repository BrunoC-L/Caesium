#include "../core/toCPP.hpp"
#include "../utility/vec_of_expected_to_expected_of_vec.hpp"

using T = typename_of_type_visitor;
using R = T::R;

R T::operator()(const Realised::Type& t) {
	return make_typename(NodeStructs::BaseTypename{ t.name._value }, NodeStructs::Value{}, copy(t.info));
}

R T::operator()(const Realised::PrimitiveType& t) {
	return typename_of_primitive(t);
}

R T::operator()(const Realised::FunctionType& t) {
	return make_typename(
		NodeStructs::BaseTypename{ t.name._value },
		NodeStructs::Value{},
		caesium_source_location{ .file_name = "function type:/" + t.name._value, .content = t.name._value }
	);
}

R T::operator()(const Realised::InterfaceType& t) {
	return make_typename(
		NodeStructs::BaseTypename{ t.interface.get().name._value },
		NodeStructs::Value{},
		copy(t.interface.get().info)
	);
}

R T::operator()(const Realised::NamespaceType& t) {
	return make_typename(
		NodeStructs::BaseTypename{ t.name_space.get().name },
		NodeStructs::Value{},
		copy(t.name_space.get().info)
	);
}

R T::operator()(const Realised::Builtin& t) {
	NOT_IMPLEMENTED;
	//return make_typename(NodeStructs::BaseTypename{ t.builtin._value }, NodeStructs::Value{}, rule_info_stub_no_throw());
}

R T::operator()(const Realised::UnionType& t) {
	expected<std::vector<NodeStructs::Typename>> vec = vec_of_expected_to_expected_of_vec(t.arguments
		| std::views::transform([&](auto&& e) { return operator()(e); })
		| to_vec()
	);
	return_if_error(vec);
	std::stringstream ss;
	bool has_prev = false;
	for (const NodeStructs::Typename& e : vec.value()) {
		if (has_prev)
			ss << " | ";
		has_prev = true;
		ss << e.info.content;
	}
	return make_typename(NodeStructs::UnionTypename{
		std::move(vec).value()
	}, NodeStructs::Value{}, caesium_source_location{ .file_name = "todo?", .content = ss.str() });
}

R T::operator()(const Realised::TemplateType& t) {
	return make_typename(NodeStructs::BaseTypename{ t.name._value }, NodeStructs::Value{}, rule_info_stub_no_throw());
}

R T::operator()(const Realised::EnumType& t) {
	return make_typename(NodeStructs::BaseTypename{ t.enum_.get().name }, NodeStructs::Value{}, rule_info_stub_no_throw());
}

R T::operator()(const Realised::EnumValueType& t) {
	return make_typename(
		NodeStructs::NamespacedTypename{
			make_typename(NodeStructs::BaseTypename{ t.enum_.get().name }, NodeStructs::Value{}, rule_info_stub_no_throw()),
			t.value_name
		}, NodeStructs::Value{}, rule_info_stub_no_throw()
	);
}

R T::operator()(const Realised::OptionalType& t) {
	auto tn_or_e = operator()(t.value_type);
	return_if_error(tn_or_e);
	return make_typename(NodeStructs::OptionalTypename{ std::move(tn_or_e).value() }, NodeStructs::Value{}, rule_info_stub_no_throw());
}

R T::operator()(const Realised::AggregateType& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const Realised::VectorType& t) {
	std::vector<NodeStructs::WordTypenameOrExpression> v;
	{
		auto inner = operator()(t.value_type);
		return_if_error(inner);
		v.push_back(NodeStructs::WordTypenameOrExpression{ std::move(inner).value() });
	}
	return make_typename(NodeStructs::TemplatedTypename{
		.type = make_typename(NodeStructs::BaseTypename{ "Vector" }, NodeStructs::Value{}, rule_info_language_element("Vector")),
		.templated_with = std::move(v)
	}, NodeStructs::Value{}, rule_info_language_element("Vector"));
}

R T::operator()(const Realised::SetType& t) {
	std::vector<NodeStructs::WordTypenameOrExpression> v;
	{
		auto inner = operator()(t.value_type);
		return_if_error(inner);
		v.push_back(NodeStructs::WordTypenameOrExpression{ std::move(inner).value() });
	}
	return make_typename(NodeStructs::TemplatedTypename{
		.type = make_typename(NodeStructs::BaseTypename{ "Set" }, NodeStructs::Value{}, rule_info_language_element("Set")),
		.templated_with = std::move(v)
	}, NodeStructs::Value{}, rule_info_language_element("Set"));
}

R T::operator()(const Realised::MapType& t) {
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

R T::operator()(const Realised::TypeListType& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const Realised::CompileTimeType& t) {
	NOT_IMPLEMENTED;
}
