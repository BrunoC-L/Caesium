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
		auto u = transpile_type_visitor{ {}, variables, named }(t);
		if (u.has_value())
			return std::unexpected{ user_error{ "Error: object of type `" + std::move(u).value() + "` has no member `" + property_name + "`\n"} };
		else
			return std::unexpected{ std::move(u).error() };
	}
	else
		return std::pair{ NodeStructs::Value{}, type_of_typename_visitor{ {}, variables, named }(pos->type) };
}
R T::operator()(const NodeStructs::TypeTemplateInstanceType& t) {
	throw;
}
R T::operator()(const NodeStructs::AggregateType& t) {
	throw;
}
R T::operator()(const NodeStructs::TypeType& t) {
	throw;
}
R T::operator()(const NodeStructs::TypeTemplateType t) {
	throw;
}
R T::operator()(const NodeStructs::FunctionType& t) {
	throw;
}
R T::operator()(const NodeStructs::FunctionTemplateType& t) {
	throw;
}
R T::operator()(const NodeStructs::UnionType& t) {
	throw;
}