#include "../core/toCPP.hpp"
#include "../utility/replace_all.hpp"

using T = transpile_member_call_visitor;
using R = T::R;

R T::operator()(const std::reference_wrapper<const NodeStructs::Type>& t_) {
	const auto& t = t_.get();
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
		if (!state.state.traversed_functions.contains(fn)) {
			state.state.traversed_functions.insert(fn);
			auto t = transpile(state.unindented(), fn);
			return_if_error(t);
			state.state.transpile_in_reverse_order.push_back(std::move(t).value());
		}
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
		if (!std::holds_alternative<non_type_information>(expr_info.value()))
			throw;
		const non_type_information& expr_info_ok = std::get<non_type_information>(expr_info.value());
		std::stringstream ss;
		ss << fn.name << "(" << expr_info_ok.representation;

		for (int i = 1; i < fn.parameters.size(); ++i) {
			auto nth_param = type_of_typename(state, fn.parameters.at(i).typename_);
			return_if_error(nth_param);
			auto nth_argument = transpile_expression(state, arguments.at(i - 1).expr);
			return_if_error(nth_argument);
			if (!std::holds_alternative<non_type_information>(nth_argument.value()))
				throw;
			const non_type_information& nth_argument_ok = std::get<non_type_information>(nth_argument.value());
			if (!is_assignable_to(state, nth_param.value(), nth_argument_ok.type.type))
				throw;
			ss << ", " << nth_argument_ok.representation;
		}
		ss << ")";

		auto return_t = type_of_typename(state, fn.returnType);
		return_if_error(return_t);

		return expression_information{ non_type_information {
			.type = return_t.value(),
			.representation = ss.str(),
			.value_category = NodeStructs::Value{},
		} };
	}
	else if (auto it = state.state.named.functions_using_auto.find(property_name); it != state.state.named.functions_using_auto.end()) {
		auto fn = realise_function_using_auto(state, *it->second.back()/*, t*/, arguments);
		return_if_error(fn);
		auto& vec = state.state.named.functions[fn.value().name];
		vec.push_back(new NodeStructs::Function(std::move(fn).value()));
		return operator()(t);
	}
	else
		return error{ "user error","Error: object of type `" + t.name + "` has no member `" + property_name + "`\n" };
	throw;
}

//R T::operator()(const NodeStructs::AggregateType& t) {
//	if (t.arguments.size() == 0)
//		throw;
//
//	/*auto category_info_pairs = arguments
//		| LIFT_TRANSFORM_X(fn_arg, std::pair{
//			fn_arg.category,
//			transpile_expression(state, fn_arg.expr)
//		})
//		| to_vec();
//
//	for (const auto& category_info : category_info_pairs)
//		if (category_info.second.has_error())
//			return category_info.second.error();
//
//	auto cat_type_pairs = category_info_pairs
//		| LIFT_TRANSFORM_X(fn_arg, std::pair{
//			fn_arg.first.has_value() ? fn_arg.first.value() : NodeStructs::ArgumentCategory{ NodeStructs::Move{} },
//			fn_arg.second.value().type
//		})
//		| to_vec();
//	;
//
//	auto args = range_join(cat_type_pairs, t.arguments);*/
//
//	//if (auto it = state.state.named.functions.find(property_name); it != state.state.named.functions.end()) {
//	//	const auto& fn = *it->second.back();
//	//	if (!state.state.traversed_functions.contains(fn)) {
//	//		state.state.traversed_functions.insert(fn);
//	//		auto t = transpile(state.unindented(), fn);
//	//		return_if_error(t);
//	//		state.state.transpile_in_reverse_order.push_back(std::move(t).value());
//	//	}
//	//	if (fn.parameters.size() != this->arguments.size() + t.arguments.size())
//	//		throw;
//
//	//	std::stringstream ss;
//	//	ss << fn.name << "(";
//
//	//	int i = 0;
//	//	for (const auto& arg : args) {
//	//		auto nth_param = type_of_typename(state, fn.parameters.at(i).typename_);
//	//		return_if_error(nth_param);
//	//		auto nth_argument = transpile_expression(state, arg.expr);
//	//		return_if_error(nth_argument);
//	//		if (!is_assignable_to(state, nth_param.value(), nth_argument.value().type))
//	//			throw;
//	//		ss << ", " << nth_argument.value().representation;
//	//		i += 1;
//	//	}
//	//	ss << ")";
//
//	//	auto return_t = type_of_typename(state, fn.returnType);
//	//	return_if_error(return_t);
//
//	//	return expression_information{
//	//		.value_category = NodeStructs::Value{},
//	//		.type = return_t.value(),
//	//		.representation = ss.str()
//	//	};
//	//}
//	//else if (auto it = state.state.named.functions_using_auto.find(property_name); it != state.state.named.functions_using_auto.end()) {
//	//	auto fn = realise_function_using_auto(state, *it->second.back()/*, t*/, args);
//	//	return_if_error(fn);
//	//	auto& vec = state.state.named.functions[fn.value().name];
//	//	vec.push_back(new NodeStructs::Function(std::move(fn).value()));
//	//	return operator()(t);
//	//}
//	//else {
//	//	auto k1 = typename_of_type(state, t);
//	//	return_if_error(k1);
//	//	auto k2 = transpile_typename(state, k1);
//	//	return_if_error(k2);
//	//	return error{ "user error","Error: object of type `" + k2.value() + "` has no member `" + property_name + "`\n"};
//	//}
//	throw;
//}

R T::operator()(const NodeStructs::PrimitiveType& t) {
	throw;
}

R T::operator()(const NodeStructs::FunctionType& t) {
	throw;
}

R T::operator()(const NodeStructs::InterfaceType& t) {
	throw;
}

R T::operator()(const NodeStructs::NamespaceType& t) {
	throw;
}

R T::operator()(const NodeStructs::UnionType& t) {
	throw;
}

R T::operator()(const NodeStructs::Template& t) {
	throw;
}

R T::operator()(const NodeStructs::Vector& t) {
	throw;
}

R T::operator()(const NodeStructs::VectorType& t) {
	auto operand_info = transpile_expression(state, expr);
	return_if_error(operand_info);
	if (!std::holds_alternative<non_type_information>(operand_info.value()))
		throw;
	const auto& operand_info_ok = std::get<non_type_information>(operand_info.value());

	auto args_repr = transpile_args(state, arguments);
	return_if_error(args_repr);

	if (property_name == "push") {
		if (arguments.size() == 0)
			throw;
		if (arguments.size() > 1)
			throw;

		auto arg_t = transpile_expression(state, arguments.at(0).expr);
		return_if_error(arg_t);
		if (!std::holds_alternative<non_type_information>(arg_t.value()))
			throw; // primitive is ok if it fits, ex: `1` can be pushed to std::vector<int>
		const auto& arg_t_ok = std::get<non_type_information>(arg_t.value());

		if (!is_assignable_to(state, t.value_type, arg_t_ok.type.type))
			throw;
		return expression_information{ non_type_information{
			.type = *state.state.named.types.at("Void").back(),
			.representation = "push(" + operand_info_ok.representation + ", " + args_repr.value() + ")",
			.value_category = NodeStructs::Value{},
		} };
	}
	if (property_name == "size") {
		if (arguments.size() != 0)
			throw;
		return expression_information{ non_type_information{
			.type = *state.state.named.types.at("Int").back(),
			.representation = operand_info_ok.representation + ".size()",
			.value_category = NodeStructs::Value{},
		} };
	}
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
