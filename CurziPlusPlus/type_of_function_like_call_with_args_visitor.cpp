#include "type_of_function_like_call_with_args_visitor.hpp"
#include "type_of_typename_visitor.hpp"

using T = type_of_function_like_call_with_args_visitor;
using R = T::R;

R T::operator()(const NodeStructs::Type& t) {
	return user_error{
		"Use of type like a function is prohibited. Type was `" + t.name + "`"
	}.unexpected();
}
R T::operator()(const NodeStructs::TypeTemplateInstanceType& t) {
	return user_error{
		"Use of type like a function is prohibited. Type was `" + t.type_template.get().templated.name + "`"
	}.unexpected();
}
R T::operator()(const NodeStructs::AggregateType& t) {
	return user_error{
		"Use of an aggregate like a function is prohibited. Aggregate was"
	}.unexpected();
}
R T::operator()(const NodeStructs::TypeType& t) {
	return user_error{
		"Use of a 'type' type like a function is prohibited, a 'type' type results from typeof(<some type>)"
	}.unexpected();
}
R T::operator()(const NodeStructs::TypeTemplateType t) {
	throw;
}
R T::operator()(const NodeStructs::FunctionType& t) {
	if (args.size() != t.function.get().parameters.size()) {
		std::stringstream ss;
		ss << t.function.get().name << " has " << t.function.get().parameters.size() << " parameters but received " << args.size() << " arguments";
		return user_error{ ss.str() }.unexpected();
	}
	else {
		// if args apply... TODO
		return std::pair{ NodeStructs::Value{}, type_of_typename_visitor{ {}, variables, named }(t.function.get().returnType) };
	}
	throw;
}
R T::operator()(const NodeStructs::FunctionTemplateType& t) {
	// we need to find the return type
	// we will insert template types into named and once the template is specialized we can treat it like a function
	throw;
}
R T::operator()(const NodeStructs::UnionType& t) {
	throw;
}