#include "type_of_function_like_call_with_args_visitor.hpp"
#include "type_of_typename_visitor.hpp"

using T = type_of_function_like_call_with_args_visitor;
using R = T::R;

R T::operator()(const NodeStructs::Type& t) {
	return error{
		"user error",
		"Use of type like a function is prohibited. Type was `" + t.name + "`"
	};
}

R T::operator()(const NodeStructs::TypeTemplateInstanceType& t) {
	return error{
		"user error",
		"Use of type like a function is prohibited. Type was `" + t.type_template.get().templated.name + "`"
	};
}

R T::operator()(const NodeStructs::AggregateType& t) {
	return error{
		"user error",
		"Use of an aggregate like a function is prohibited. Aggregate was"
	};
}

R T::operator()(const NodeStructs::TypeType& t) {
	return error{
		"user error",
		"Use of a 'type' type like a function is prohibited, a 'type' type results from typeof(<some type>)"
	};
}

R T::operator()(const NodeStructs::TypeTemplateType t) {
	throw;
}

R T::operator()(const NodeStructs::FunctionType& t) {
	if (args.size() != t.function.get().parameters.size()) {
		std::stringstream ss;
		ss << t.function.get().name << " has " << t.function.get().parameters.size() << " parameters but received " << args.size() << " arguments";
		return error{ "user error", ss.str() };
	}
	else {
		// if args apply... TODO
		return std::pair{ NodeStructs::Value{}, type_of_typename_visitor{ {}, state }(t.function.get().returnType).value() };
	}
	throw;
}

R T::operator()(const NodeStructs::FunctionTemplateType& t) {
	// we insert template arguments into named and once the template is specialized we can treat it like a function
	const auto& fn = create_or_retrieve_instance(state, t.function_template, args); 
	throw;
}

R T::operator()(const NodeStructs::FunctionTemplateInstanceType& t) {
	// if the return type comes from the template this won't work
	// we can iterate the template parameters and check if the names match with the names used in the return type typename to detect if we need to do more work
	auto res = type_of_typename_visitor{ {}, state }(t.function_template.get().templated.returnType);
	return_if_error(res);
	return std::pair{
		NodeStructs::Value{},
		std::move(res).value()
	};
}

R T::operator()(const NodeStructs::UnionType& t) {
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
