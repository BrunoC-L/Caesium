#include "../core/toCPP.hpp"
#include "../utility/vec_of_expected_to_expected_of_vec.hpp"

using T = name_of_type_visitor;
using R = T::R;

R T::operator()(const Realised::Type& t) {
	return t.name._value;
}

R T::operator()(const Realised::PrimitiveType& t) {
	return name_of_primitive(t);
}

R T::operator()(const Realised::FunctionType& t) {
	return t.name._value;
}

R T::operator()(const Realised::Interface& t) {
	return t.name._value;
}

R T::operator()(const Realised::NamespaceType& t) {
	return t.name._value;
}

R T::operator()(const Realised::Builtin& t) {
	NOT_IMPLEMENTED;
	//return make_typename(NodeStructs::BaseTypename{ t.builtin._value }, NodeStructs::Value{}, rule_info_stub_no_throw());
}

R T::operator()(const Realised::UnionType& t) {
	expected<std::vector<std::string>> vec = vec_of_expected_to_expected_of_vec(t.arguments
		| std::views::transform([&](auto&& e) { return operator()(e); })
		| to_vec()
	);
	return_if_error(vec);

	std::sort(
		vec.value().begin(),
		vec.value().end()
	);
	std::stringstream ss_res;
	std::stringstream ss_cpp;
	bool has_prev = false;
	for (const std::string& e : vec.value()) {
		if (has_prev) {
			ss_res << "_or_";
			ss_cpp << ", ";
		}
		has_prev = true;
		ss_res << e;
		ss_cpp << e;
	}
	std::string res = "Union_" + ss_res.str() + "_";
	std::string incpp = "Union<" + ss_cpp.str() + ">";
	state.state.types.insert({ res, { copy(t) } });
	state.state.aliases_to_transpile.insert({ res, std::move(incpp) });
	return res;
}

R T::operator()(const Realised::TemplateType& t) {
	return t.name._value;
}

R T::operator()(const Realised::EnumType& t) {
	return t.name._value;
}

R T::operator()(const Realised::EnumValueType& t) {
	return t.full_name;
}

R T::operator()(const Realised::OptionalType& t) {
	auto tn_or_e = operator()(t.value_type);
	return_if_error(tn_or_e);
	NOT_IMPLEMENTED;
	//return make_typename(NodeStructs::OptionalTypename{ std::move(tn_or_e).value() }, NodeStructs::Value{}, rule_info_stub_no_throw());
}

R T::operator()(const Realised::AggregateType& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const Realised::VectorType& t) {
	auto inner = operator()(t.value_type);
	return_if_error(inner);
	std::string res = "Vector_" + inner.value() + "_";
	std::string incpp = "Vector<" + inner.value() + ">";
	state.state.types.insert({ res, { copy(t) } });
	state.state.aliases_to_transpile.insert({ res, std::move(incpp) });
	return res;
}

R T::operator()(const Realised::SetType& t) {
	auto inner = operator()(t.value_type);
	return_if_error(inner);
	std::string res = "Set_" + inner.value() + "_";
	std::string incpp = "Set<" + inner.value() + ">";
	state.state.types.insert({ res, { copy(t) } });
	state.state.aliases_to_transpile.insert({ res, std::move(incpp) });
	return res;
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
	NOT_IMPLEMENTED;
	/*return make_typename(NodeStructs::TemplatedTypename{
		.type = make_typename(NodeStructs::BaseTypename{ "Map" }, NodeStructs::Value{}, rule_info_stub_no_throw()),
		.templated_with = std::move(v)
	}, NodeStructs::Value{}, rule_info_stub_no_throw());*/
}

R T::operator()(const Realised::TypeListType& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const Realised::CompileTimeType& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const Realised::TemplateInstanceType& t) {
	NOT_IMPLEMENTED;
}
