#include "../core/toCPP.hpp"

using T = type_of_postfix_member_visitor;
using R = T::R;

R T::operator()(const NodeStructs::Type& t) {
	if (auto it = std::find_if(
		t.member_variables.begin(),
			t.member_variables.end(),
			[&](const auto& m) { return m.name == property_name; }
		); it != t.member_variables.end())
		return type_of_typename(state, variables, it->type)
			.transform([](NodeStructs::MetaType&& val) { return R::value_type{ NodeStructs::Value{}, std::move(val) }; });
	else if (auto it = state.state.global_namespace.functions.find(property_name); it != state.state.global_namespace.functions.end()) {
		const auto& fn = it->second.back();
		throw;
	}
	else
		return error{ "user error","Error: object of type `" + t.name + "` has no member `" + property_name + "`\n" };
}

R T::operator()(const NodeStructs::PrimitiveType& t) {
	throw;
}

//R T::operator()(const NodeStructs::AggregateType& t) {
//	if (t.arguments.size() == 0)
//		throw;
//	if (t.arguments.size() == 1)
//		return operator()(t.arguments.at(0).second);
//	if (auto it = state.state.global_namespace.functions.find(property_name); it != state.state.global_namespace.functions.end()) {
//		throw;
//	}
//}
//
//R T::operator()(const NodeStructs::TypeType& t) {
//	throw;
//}

R T::operator()(const NodeStructs::FunctionType& t) {
	throw;
}

R T::operator()(const NodeStructs::InterfaceType& t) {
	if (auto it = std::find_if(
		t.interface.get().member_variables.begin(),
		t.interface.get().member_variables.end(),
		[&](const auto& member) { return member.name == property_name; }
	); it != t.interface.get().member_variables.end())
		return type_of_typename(state, variables, it->type)
			.transform([](auto&& t) { return R::value_type{ NodeStructs::Reference{}, std::move(t) }; });
	else
		return error{ "user error", "Error: object of type `" + t.interface.get().name + "` has no member `" + property_name + "`\n" };
}

R T::operator()(const NodeStructs::NamespaceType& t) {
	throw;
}

R T::operator()(const NodeStructs::Builtin& t) {
	throw;
}

R T::operator()(const NodeStructs::UnionType& t) {
	throw;
}

R T::operator()(const NodeStructs::TemplateType& t) {
	throw;
}

R T::operator()(const NodeStructs::EnumType& tmpl) {
	throw;
}

R T::operator()(const NodeStructs::EnumValueType& tmpl) {
	throw;
}

R T::operator()(const NodeStructs::OptionalType& t) {
	throw;
}

R T::operator()(const NodeStructs::AggregateType& t) {
	throw;
}

R T::operator()(const NodeStructs::Vector& t) {
	throw;
}

R T::operator()(const NodeStructs::VectorType& t) {
	/*if (this->property_name == "push")
		return std::pair{
			NodeStructs::Value{},
				NodeStructs::MetaType{
					NodeStructs::BuiltInType{
						NodeStructs::BuiltInType::push_t{
							t
						}
					}
				}
			};*/
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
