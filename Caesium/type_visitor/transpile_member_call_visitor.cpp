#include "../core/toCPP.hpp"
#include "../utility/replace_all.hpp"

using T = transpile_member_call_visitor;
using R = T::R;

R T::operator()(const NodeStructs::Type& t) {
	if (auto it = std::find_if(
		t.memberVariables.begin(),
		t.memberVariables.end(),
		[&](const auto& m) { return m.name == property_name; }
	); it != t.memberVariables.end()) {
		auto mt = type_of_typename(state, it->type);
		return_if_error(mt);
		auto expect_error = type_of_function_like_call_with_args(state, arguments, mt.value());
		return_if_error(expect_error);
		throw;
	}
	else if (auto it = state.state.named.functions.find(property_name); it != state.state.named.functions.end()) {
		const auto& fn = *it->second.back();
		auto first_param_str = transpile_typename(state, fn.parameters.at(0).typename_);
		return_if_error(first_param_str);
		auto first_param = type_of_typename(state, fn.parameters.at(0).typename_);
		return_if_error(first_param);
		if (!is_assignable_to(state, first_param.value(), { t }))
			return error{ "user error", "Error: object of type `" + t.name + "` is not assignable to `" + first_param_str.value() + "`\n"};

		if (arguments.size() + 1 != fn.parameters.size())
			throw;

		auto expr_info = transpile_expression(state, expr);
		return_if_error(expr_info);
		std::stringstream ss;
		ss << fn.name << "(" << expr_info.value().representation;

		for (int i = 1; i < fn.parameters.size(); ++i) {
			auto nth_param = type_of_typename(state, fn.parameters.at(i).typename_);
			return_if_error(nth_param);
			auto nth_argument = transpile_expression(state, arguments.at(i - 1).expr);
			return_if_error(nth_argument);
			if (!is_assignable_to(state, nth_param.value(), nth_argument.value().type))
				throw;
			ss << ", " << nth_argument.value().representation;
		}
		ss << ")";

		auto return_t = type_of_typename(state, fn.returnType);
		return_if_error(return_t);

		return whole_expression_information{
			.value_category = NodeStructs::Value{},
			.type = return_t.value(),
			.representation = ss.str()
		};
	}
	else
		return error{ "user error","Error: object of type `" + t.name + "` has no member `" + property_name + "`\n" };
	throw;
}

R T::operator()(const NodeStructs::AggregateType& t) {
	if (t.arguments.size() == 0)
		throw;
	if (t.arguments.size() == 1)
		return operator()(t.arguments.at(0).second);
	throw;
}

R T::operator()(const NodeStructs::TypeType& t) {
	throw;
}

R T::operator()(const NodeStructs::FunctionType& t) {
	throw;
}

R T::operator()(const NodeStructs::InterfaceType& t) {
	throw;
}

R T::operator()(const NodeStructs::UnionType& t) {
	throw;
}

R T::operator()(const NodeStructs::VectorType& t) {
	auto operand_info = transpile_expression(state, expr);
	return_if_error(operand_info);

	auto args_repr = transpile_args(state, arguments);
	return_if_error(args_repr);

	if (arguments.size() == 0)
		throw;
	if (arguments.size() > 1)
		throw;

	auto arg_t = transpile_expression(state, arguments.at(0).expr);
	return_if_error(arg_t);

	if (!is_assignable_to(state, t.value_type, arg_t.value().type))
		throw;

	if (property_name == "push") {
		return whole_expression_information{
			.value_category = NodeStructs::Value{},
			.type = *state.state.named.types.at("Void").back(),
			.representation = "push(" + operand_info.value().representation + ", " + args_repr.value() + ")"
		};
	}
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

R T::operator()(const NodeStructs::BuiltInType& t) {
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
