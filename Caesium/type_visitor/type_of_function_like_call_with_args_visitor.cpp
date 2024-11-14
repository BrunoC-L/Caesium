#include "../core/toCPP.hpp"

using T = type_of_function_like_call_with_args_visitor;
using R = T::R;

R T::operator()(const NodeStructs::Type& t) {
	return error{
		"user error",
		"Use of type like a function is prohibited. Type was `" + t.name + "`"
	};
}

R T::operator()(const NodeStructs::PrimitiveType& t) {
	return error{
		"user error",
		"Use of type like a function is prohibited. Type was `" + transpile_typename(state, typename_of_primitive(t)).value() + "`"
	};
}

R T::operator()(const NodeStructs::FunctionType& t) {
	//if (args.size() != t.function.get().parameters.size()) {
	//	std::stringstream ss;
	//	ss << t.function.get().name << " has " << t.function.get().parameters.size() << " parameters but received " << args.size() << " arguments";
	//	return error{ "user error", ss.str() };
	//}
	//else {
	//	// if args apply... TODO
	//	auto ret = type_of_typename(state, t.function.get().returnType);
	//	return_if_error(ret);
	//	return std::pair{ NodeStructs::Value{}, ret.value() };
	//}
	throw;
}

R T::operator()(const NodeStructs::InterfaceType& t) {
	throw;
}

R T::operator()(const NodeStructs::NamespaceType& t) {
	throw;
}

R T::operator()(const NodeStructs::Builtin& t) {
	throw;
}

R T::operator()(const NodeStructs::TupleType& t) {
	throw;
}

R T::operator()(const NodeStructs::UnionType& t) {
	throw;
}

R T::operator()(const NodeStructs::TemplateType& tmpl) {
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

R T::operator()(const NodeStructs::CompileTimeType& t) {
	throw;
}
