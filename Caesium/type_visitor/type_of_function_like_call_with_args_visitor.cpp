#include "../core/toCPP.hpp"

using T = type_of_function_like_call_with_args_visitor;
using R = T::R;

R T::operator()(const Realised::Type& t) {
	return error{
		"user error",
		"Use of type like a function is prohibited. Type was `" + t.name._value + "`"
	};
}

R T::operator()(const Realised::PrimitiveType& t) {
	auto t_or_e = type_of_typename(state, variables, typename_of_primitive(t));
	return_if_error(t_or_e);
	transpile_t name = name_of_type(state, t_or_e.value());
	return_if_error(name);
	return error{
		"user error",
		"Use of type like a function is prohibited. Type was `" + name.value() + "`"
	};
}

R T::operator()(const Realised::FunctionType& t) {
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

R T::operator()(const Realised::Interface& t) {
	NOT_IMPLEMENTED;
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

R T::operator()(const Realised::TemplateType& tmpl) {
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

R T::operator()(const Realised::TemplateInstanceType& t) {
	NOT_IMPLEMENTED;
}
