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
		"Use of type like a function is prohibited. Type was `" + transpile_typename(state, variables, typename_of_primitive(t)).value() + "`"
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
	NOT_IMPLEMENTED;
}

R T::operator()(const NodeStructs::InterfaceType& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const NodeStructs::NamespaceType& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const NodeStructs::Builtin& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const NodeStructs::UnionType& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const NodeStructs::TemplateType& tmpl) {
	NOT_IMPLEMENTED;
}

R T::operator()(const NodeStructs::EnumType& tmpl) {
	NOT_IMPLEMENTED;
}

R T::operator()(const NodeStructs::EnumValueType& tmpl) {
	NOT_IMPLEMENTED;
}

R T::operator()(const NodeStructs::OptionalType& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const NodeStructs::AggregateType& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const NodeStructs::Vector& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const NodeStructs::VectorType& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const NodeStructs::Set& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const NodeStructs::SetType& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const NodeStructs::Map& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const NodeStructs::MapType& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const NodeStructs::TypeList& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const NodeStructs::TypeListType& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const NodeStructs::TypeToken& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const NodeStructs::CompileTimeType& t) {
	NOT_IMPLEMENTED;
}
