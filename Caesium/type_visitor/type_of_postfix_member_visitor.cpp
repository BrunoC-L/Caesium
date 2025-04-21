#include "../core/toCPP.hpp"

using T = type_of_postfix_member_visitor;
using R = T::R;

R T::operator()(const Realised::Type& t) {
	if (auto it = std::find_if(
		t.member_variables.begin(),
			t.member_variables.end(),
			[&](const auto& m) { return m.name._value == property_name; }
		); it != t.member_variables.end())
		NOT_IMPLEMENTED;
	/*return type_of_typename(state, variables, it->type)
			.transform([](Realised::MetaType&& val) { return R::value_type{ NodeStructs::Value{}, std::move(val) }; });*/
	else if (auto it = state.state.global_namespace.functions.find(property_name); it != state.state.global_namespace.functions.end()) {
		// const auto& fn = it->second.back();
		NOT_IMPLEMENTED;
	}
	else
		return error{ "user error","Error: object of type `" + t.name._value + "` has no member `" + property_name + "`\n" };
}

R T::operator()(const Realised::PrimitiveType& t) {
	NOT_IMPLEMENTED;
}

//R T::operator()(const NodeStructs::AggregateType& t) {
//	if (t.arguments.size() == 0)
//		NOT_IMPLEMENTED;
//	if (t.arguments.size() == 1)
//		return operator()(t.arguments.at(0).second);
//	if (auto it = state.state.global_namespace.functions.find(property_name); it != state.state.global_namespace.functions.end()) {
//		NOT_IMPLEMENTED;
//	}
//}
//
//R T::operator()(const NodeStructs::TypeType& t) {
//	NOT_IMPLEMENTED;
//}

R T::operator()(const Realised::FunctionType& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const Realised::InterfaceType& t) {
	if (auto it = std::find_if(
		t.interface.get().member_variables.begin(),
		t.interface.get().member_variables.end(),
		[&](const auto& member) { return member.name._value == property_name; }
	); it != t.interface.get().member_variables.end())
		NOT_IMPLEMENTED;
	/*return type_of_typename(state, variables, it->type)
			.transform([](auto&& t) { return R::value_type{ NodeStructs::Reference{}, std::move(t) }; });*/
	else
		return error{ "user error", "Error: object of type `" + t.interface.get().name._value + "` has no member `" + property_name + "`\n" };
}

R T::operator()(const Realised::NamespaceType& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const Realised::Builtin& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const Realised::UnionType& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const Realised::TemplateType& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const Realised::EnumType& tmpl) {
	NOT_IMPLEMENTED;
}

R T::operator()(const Realised::EnumValueType& tmpl) {
	NOT_IMPLEMENTED;
}

R T::operator()(const Realised::OptionalType& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const Realised::AggregateType& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const Realised::VectorType& t) {
	/*if (this->property_name == "push")
		return std::pair{
			NodeStructs::Value{},
				Realised::MetaType{
					NodeStructs::BuiltInType{
						NodeStructs::BuiltInType::push_t{
							t
						}
					}
				}
			};*/
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
