#include "type_of_template_instantiation_with_args_visitor.hpp"
#include "type_of_typename_visitor.hpp"
#include "type_of_expression_visitor.hpp"
#include "vec_of_expected_to_expected_of_vec.hpp"
#include "type_of_typename_visitor.hpp"
#include "transpile_typename_visitor.hpp"

using T = type_of_template_instantiation_with_args_visitor;
using R = T::R;

R T::operator()(const NodeStructs::Type& t) {
	throw;
}

R T::operator()(const NodeStructs::AggregateType& t) {
	throw;
}

R T::operator()(const NodeStructs::TypeType& t) {
	throw;
}

R T::operator()(const NodeStructs::FunctionType& t) {
	throw;
}
//
//R T::operator()(const NodeStructs::FunctionTemplateType& t) {
//	std::vector<expected<std::pair<NodeStructs::ValueCategory, NodeStructs::UniversalType>>> v1 = args
//		| LIFT_TRANSFORM(type_of_expression_visitor{ {}, state })
//		| to_vec()
//		;
//	auto v2 = vec_of_expected_to_expected_of_vec<std::pair<NodeStructs::ValueCategory, NodeStructs::UniversalType>, std::vector>(v1);
//	return_if_error(v2);
//
//	std::vector<NodeStructs::UniversalType> arg_types = std::move(v2).value() | LIFT_TRANSFORM_TRAIL(.second) | to_vec();
//
//	const auto& f_tmpl = t.function_template;
//
//	// check if args are valid
//	if (f_tmpl.get().parameters.parameters.size() != arg_types.size())
//		throw;
//
//	for (unsigned i = 0; i < arg_types.size(); ++i) {
//		const auto& arg = arg_types.at(i);
//		const auto& template_parameter = f_tmpl.get().parameters.parameters.at(i);
//		// replace all template parameter instances with the arg
//		// call the compiler here? with the string representing the function?
//		// replace, tokenize, 
//		throw;
//	}
//
//
//
//	/*if (!state.state.traversed_function_templates.contains(f)) {
//		state.state.traversed_function_templates.insert(f);
//		auto template_definition = transpile(state.unindented(), f);
//		return_if_error(template_definition);
//		state.state.transpile_in_reverse_order.push_back(std::move(template_definition).value());
//	}*/
//
//	return NodeStructs::UniversalType{ NodeStructs::FunctionTemplateInstanceType{
//		.function_template = t.function_template,
//		.template_arguments = arg_types
//	} };
//}

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

// https://stackoverflow.com/a/24315631/10945691
static std::string ReplaceAll(std::string str, const std::string& from, const std::string& to) {
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
	}
	return str;
}

R T::operator()(const NodeStructs::Template& tmpl) {
	
	if (tmpl.parameters.size() != templated_with.size()) {
		std::stringstream ss;
		ss << "invalid number of arguments to template `"
			<< tmpl.name
			<< "`, expected `"
			<< tmpl.parameters.size()
			<< "`, received `"
			<< templated_with.size()
			<< "`";
		return error{ "user error", ss.str() };
	}

	if (tmpl.templated == "BUILTIN") {
		/*if (tmpl.name == "Vector") {
			auto x = type_of_typename_visitor{ {}, state }(templated_with.at(0));
			return_if_error(x);
			return NodeStructs::UniversalType{ NodeStructs::VectorType{ Box<NodeStructs::UniversalType>{ std::move(x).value() } } };
		}
		if (tmpl.name == "Set") {
			auto x = type_of_typename_visitor{ {}, state }(templated_with.at(0));
			return_if_error(x);
			return NodeStructs::UniversalType{ NodeStructs::SetType{ Box<NodeStructs::UniversalType>{ std::move(x).value() } } };
		}
		if (tmpl.name == "Map") {
			auto x = type_of_typename_visitor{ {}, state }(templated_with.at(0));
			auto y = type_of_typename_visitor{ {}, state }(templated_with.at(1));
			return_if_error(x);
			return_if_error(y);
			return NodeStructs::UniversalType{ NodeStructs::MapType{
				Box<NodeStructs::UniversalType>{ std::move(x).value() },
				Box<NodeStructs::UniversalType>{ std::move(y).value() }
			} };
		}*/
		throw;
	}

	// todo check if already traversed

	std::string replaced = tmpl.templated;
	for (int i = 0; i < tmpl.parameters.size(); ++i) {
		/*auto e = transpile_typename_visitor{ {}, state }(templated_with.at(i));
		return_if_error(e);
		replaced = ReplaceAll(std::move(replaced), tmpl.parameters.at(i), e.value());*/
	}

	/*Type t{ 1 };
	auto tokens = Tokenizer(replaced).read();
	tokens_and_iterator g{ tokens, tokens.begin() };
	bool ok = t.build(g) && g.it == g.tokens.end();
	if (!ok)*/
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
