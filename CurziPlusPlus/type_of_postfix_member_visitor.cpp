#include "type_of_function_like_call_with_args_visitor.hpp"
#include "type_of_postfix_member_visitor.hpp"
#include "type_of_typename_visitor.hpp"
#include "transpile_type_visitor.hpp"

using T = type_of_postfix_member_visitor;
using R = T::R;

R T::operator()(const NodeStructs::Type& t) {
	const auto& v = t.memberVariables;
	auto pos = std::find_if(v.begin(), v.end(), [&](const NodeStructs::MemberVariable& m) { return m.name == property_name; });
	if (pos == v.end()) {
		auto u = transpile_type_visitor{ {}, state }(t);
		if (u.has_value())
			return error{ "user error","Error: object of type `" + std::move(u).value() + "` has no member `" + property_name + "`\n"};
		else
			return std::move(u).error();
	}
	else
		return type_of_typename_visitor{ {}, state }(pos->type).transform([](auto&& val) { return R::value_type{ NodeStructs::Value{}, std::move(val) }; });
}
//
//R T::operator()(const NodeStructs::TypeTemplateInstanceType& t) {
//	// basically trying to implement transpilation of Vector<T>{}.size()
//	// Vector<T> is the type template instance and `size` is `property_name`
//	if (auto it = state.state.named.function_templates.find(property_name); it != state.state.named.function_templates.end()) {
//		return std::pair<NodeStructs::ParameterCategory, NodeStructs::UniversalType>{ NodeStructs::Value{}, NodeStructs::FunctionTemplateType{
//			.function_template = *it->second.back()
//		} };
//	}
//	throw;
//}

R T::operator()(const NodeStructs::AggregateType& t) {
	throw;
}

R T::operator()(const NodeStructs::TypeType& t) {
	throw;
}

R T::operator()(const NodeStructs::FunctionType& t) {
	throw;
}

R T::operator()(const NodeStructs::UnionType& t) {
	throw;
}

R T::operator()(const NodeStructs::VectorType& t) {
	throw;
}

R T::operator()(const NodeStructs::SetType& t) {
	throw;
}

R T::operator()(const NodeStructs::MapType& t) {
	throw;
}

R T::operator()(const NodeStructs::Template& t) {
	throw;
}

R T::operator()(const std::string&) {
	throw;
}

R T::operator()(const double&) {
	throw;
}

R T::operator()(const int&) {
	throw;
}

R T::operator()(const bool&) {
	throw;
}
