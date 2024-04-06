#include "../core/toCPP.hpp"
#include "../utility/replace_all.hpp"

using T = transpile_member_call_visitor;
using R = T::R;

R T::operator()(const NodeStructs::Type& t) {
	if (auto it = std::find_if(
		t.member_variables.begin(),
		t.member_variables.end(),
		[&](const auto& m) { return m.name == property_name; }
	); it != t.member_variables.end()) {
		auto mt = type_of_typename(state, it->type);
		return_if_error(mt);
		auto expect_error = type_of_function_like_call_with_args(state, arguments, mt.value());
		return_if_error(expect_error);
		throw;
	}

	if (auto it = state.state.global_namespace.functions.find(property_name); it != state.state.global_namespace.functions.end()) {
		const auto& fn = it->second.back();
		if (!state.state.traversed_functions.contains(fn)) {
			state.state.traversed_functions.insert(fn);
			state.state.functions_to_transpile.insert(fn);
		}
		auto first_param_str = transpile_typename(state, fn.parameters.at(0).typename_);
		return_if_error(first_param_str);
		auto first_param = type_of_typename(state, fn.parameters.at(0).typename_);
		return_if_error(first_param);
		if (!is_assignable_to(state, first_param.value(), { t }))
			return error{ "user error", "Error: object of type `" + t.name + "` is not assignable to `" + first_param_str.value() + "`\n" };

		if (arguments.size() + 1 != fn.parameters.size())
			throw;

		std::stringstream ss;
		ss << fn.name << "(" << operand_info.representation;

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

		return expression_information{ non_type_information{
			.type = return_t.value(),
			.representation = ss.str(),
			.value_category = NodeStructs::Value{},
		} };
	}

	if (auto it = state.state.global_namespace.functions_using_auto.find(property_name); it != state.state.global_namespace.functions_using_auto.end()) {
		auto fn = realise_function_using_auto(state, it->second.back(), arguments);
		return_if_error(fn);
		auto& vec = state.state.global_namespace.functions[fn.value().name];
		bool found = false;
		for (const auto& f : vec)
			if (f <=> fn.value() == std::weak_ordering::equivalent)
				found = true;
		if (!found)
			vec.push_back(std::move(fn).value());
		return operator()(t);
	}
	if (auto it = state.state.global_namespace.builtins.find(property_name); it != state.state.global_namespace.builtins.end()) {
		const std::string& name = it->second.back().name;
		if (name == "size") {
			/*if (auto is_str = t <=> NodeStructs::MetaType{ NodeStructs::PrimitiveType{ { std::string{} } } } == std::weak_ordering::equivalent) {
				return expression_information{ non_type_information{
					.type = NodeStructs::PrimitiveType{ { int{} } },
					.representation = operand_info.representation + ".size()",
					.value_category = NodeStructs::Value{}
				} };
			}*/
			throw;
		}
		throw;
	}
	return error{ "user error","Error: object of type `" + t.name + "` has no member `" + property_name + "`\n" };
}

R T::operator()(const NodeStructs::PrimitiveType& t) {
	if (std::holds_alternative<std::string>(t.value)) {
		if (property_name == "size") {
			if (arguments.size() != 0)
				throw;
			return expression_information{ non_type_information{
				.type = NodeStructs::MetaType{ NodeStructs::PrimitiveType{ { int{} } } },
				.representation = operand_info.representation + ".size()",
				.value_category = NodeStructs::Value{},
			} };
		}
	}
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

R T::operator()(const NodeStructs::Builtin& t) {
	throw;
}

R T::operator()(const NodeStructs::UnionType& t) {
	throw;
}

R T::operator()(const NodeStructs::TemplateType& t) {
	throw;
}

R T::operator()(const NodeStructs::EnumType& t) {
	throw;
}

R T::operator()(const NodeStructs::EnumValueType& t) {
	throw;
}

R T::operator()(const NodeStructs::AggregateType& t) {
	throw;
}

R T::operator()(const NodeStructs::Vector& t) {
	throw;
}

R T::operator()(const NodeStructs::VectorType& t) {
	/*auto args_repr = transpile_args(state, arguments);
	return_if_error(args_repr);*/

	if (property_name == "push") {
		if (arguments.size() != 1)
			throw;

		//todo check value cat
		auto arg_info = transpile_expression(state, arguments.at(0).expr);
		return_if_error(arg_info);
		if (!std::holds_alternative<non_type_information>(arg_info.value()))
			throw;
		const auto& arg_info_ok = std::get<non_type_information>(arg_info.value());
		auto repr = transpile_arg(state, arguments.at(0));
		return_if_error(repr);

		//todo check conversion
		if (!is_assignable_to(state, t.value_type.get(), arg_info_ok.type.type))
			return error{
				"user error",
				"wrong type pushed to Vector<T>"
		};
		return expression_information{ non_type_information{
			.type = t,
			.representation = "push(" + operand_info.representation + ", " + repr.value() + ")",
			.value_category = NodeStructs::Value{}
		} };
	}

	if (property_name == "size") {
		if (arguments.size() != 0)
			throw;
		return expression_information{ non_type_information{
			.type = NodeStructs::PrimitiveType{ { int{} } },
			.representation = operand_info.representation + ".size()",
			.value_category = NodeStructs::Value{}
		} };
	}

	if (property_name == "reserve") {
		if (arguments.size() != 1)
			throw;

		//todo check value cat
		auto arg_info = transpile_expression(state, arguments.at(0).expr);
		return_if_error(arg_info);
		if (!std::holds_alternative<non_type_information>(arg_info.value()))
			throw;
		const auto& arg_info_ok = std::get<non_type_information>(arg_info.value());
		auto repr = transpile_arg(state, arguments.at(0));
		return_if_error(repr);

		//todo check conversion
		if (arg_info_ok.type.type <=> NodeStructs::MetaType{ NodeStructs::PrimitiveType{ { int{} } } }  != std::weak_ordering::equivalent)
			return error{
				"user error",
				"wrong type for vector reserve"
			};
		return expression_information{ non_type_information{
			.type = NodeStructs::PrimitiveType{ { NodeStructs::void_t{} } },
			.representation = operand_info.representation + ".reserve(" + repr.value() + ")",
			.value_category = NodeStructs::Value{}
		} };
	}

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
