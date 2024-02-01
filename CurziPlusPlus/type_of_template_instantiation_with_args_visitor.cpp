#include "type_of_template_instantiation_with_args_visitor.hpp"
#include "type_of_typename_visitor.hpp"
#include "type_of_expression_visitor.hpp"
#include "vec_of_expected_to_expected_of_vec.hpp"

using T = type_of_template_instantiation_with_args_visitor;
using R = T::R;

R T::operator()(const NodeStructs::Type& t) {
	throw;
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
	std::vector<expected<std::pair<NodeStructs::ValueCategory, NodeStructs::UniversalType>>> v1 = args
		| LIFT_TRANSFORM(type_of_expression_visitor{ {}, state })
		| to_vec()
		;
	auto v2 = vec_of_expected_to_expected_of_vec<std::pair<NodeStructs::ValueCategory, NodeStructs::UniversalType>, std::vector>(v1);
	return_if_error(v2);

	std::vector<NodeStructs::UniversalType> arg_types = std::move(v2).value() | LIFT_TRANSFORM_TRAIL(.second) | to_vec();

	const auto& f_tmpl = t.function_template;

	// check if args are valid
	if (f_tmpl.get().parameters.parameters.size() != arg_types.size())
		throw;

	for (unsigned i = 0; i < arg_types.size(); ++i) {
		const auto& arg = arg_types.at(i);
		const auto& template_parameter = f_tmpl.get().parameters.parameters.at(i);
		// replace all template parameter instances with the arg
		// call the compiler here? with the string representing the function?
		// replace, tokenize, 
		throw;
	}



	/*if (!state.state.traversed_function_templates.contains(f)) {
		state.state.traversed_function_templates.insert(f);
		auto template_definition = transpile(state.unindented(), f);
		return_if_error(template_definition);
		state.state.transpile_in_reverse_order.push_back(std::move(template_definition).value());
	}*/

	return NodeStructs::UniversalType{ NodeStructs::FunctionTemplateInstanceType{
		.function_template = t.function_template,
		.template_arguments = arg_types
	} };
}

R T::operator()(const NodeStructs::FunctionTemplateInstanceType& t) {
	throw;
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
