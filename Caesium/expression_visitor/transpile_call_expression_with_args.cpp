//#include "../core/toCPP.hpp"
//#include "../utility/vec_of_expected_to_expected_of_vec.hpp"
//
//using T = transpile_call_expression_with_args_visitor;
//using R = T::R;
//
//R base(
//	transpilation_state_with_indent state,
//	const std::vector<NodeStructs::FunctionArgument>& arguments,
//	const auto& expr
//) {
//	auto operand_t = transpile_expression(state, expr);
//	return_if_error(operand_t);
//	if (!std::holds_alternative<type_information>(operand_t.value()))
//		throw;
//	const type_information& operand_t_ok = std::get<type_information>(operand_t.value());
//
//	if (std::holds_alternative<NodeStructs::Template>(operand_t_ok.type.type)) {
//		const auto& tmpl = std::get<NodeStructs::Template>(operand_t_ok.type.type);
//		auto arg_ts = vec_of_expected_to_expected_of_vec(
//			arguments
//			| LIFT_TRANSFORM_TRAIL(.expr)
//			| LIFT_TRANSFORM_X(X, transpile_expression(state, X))
//			| to_vec()
//		);
//		if (tmpl.templated == "BUILTIN") {
//			if (tmpl.name == "print" || tmpl.name == "println") {
//				std::stringstream ss;
//
//				ss << "(Void)(std::cout";
//				for (const auto& [_, arg] : arguments) {
//					auto expr_s = expr_to_printable(state, arg);
//					return_if_error(expr_s);
//					ss << " << " << expr_s.value();
//				}
//				bool newline = tmpl.name == "println";
//				if (newline)
//					ss << " << \"\\n\"";
//				ss << ")";
//				return expression_information{ non_type_information{
//					.type = *state.state.named.types.at("Void").back(),
//					.representation = ss.str(),
//					.value_category = NodeStructs::Value{}
//				} };
//			}
//			throw;
//		}
//		throw;
//	}
//	if (std::holds_alternative<NodeStructs::FunctionType>(operand_t_ok.type.type)) {
//		const auto& fn = std::get<NodeStructs::FunctionType>(operand_t_ok.type.type).function.get();
//		if (!state.state.traversed_functions.contains(fn)) {
//			state.state.traversed_functions.insert(fn);
//			auto t = transpile(state.unindented(), fn);
//			return_if_error(t);
//			state.state.transpile_in_reverse_order.push_back(std::move(t).value());
//		}
//		auto args_or_error = transpile_args(state, arguments);
//		return_if_error(args_or_error);
//		return expression_information{ non_type_information{
//			.type = type_of_typename(state, fn.returnType).value(),
//			.representation = std::move(operand_t_ok).representation + "(" + std::move(args_or_error).value() + ")",
//			.value_category = NodeStructs::Value{}
//		} };
//	}
//
//	auto t = type_of_function_like_call_with_args(state, arguments, operand_t_ok.type);
//	return_if_error(t);
//	auto args_or_error = transpile_args(state, arguments);
//	return_if_error(args_or_error);
//	return expression_information{ non_type_information{
//		.type = t.value().second,
//		.representation = std::move(operand_t_ok).representation + "(" + std::move(args_or_error).value() + ")",
//		.value_category = NodeStructs::Value{}
//	} };
//}
////
////transpile_t2 transpile_call_expression_with_args(
////	transpilation_state_with_indent state,
////	const std::vector<NodeStructs::FunctionArgument>& arguments,
////	const NodeStructs::Typename& namespaced_function
////) {
////	auto operand_t = type_of_typename(state, namespaced_function);
////	return_if_error(operand_t);
////
////	if (std::holds_alternative<NodeStructs::Template>(operand_t.value().type)) {
////		const auto& tmpl = std::get<NodeStructs::Template>(operand_t.value().type);
////		auto arg_ts = vec_of_expected_to_expected_of_vec(
////			arguments
////			| LIFT_TRANSFORM_TRAIL(.expr)
////			| LIFT_TRANSFORM_X(X, transpile_expression(state, X))
////			| to_vec()
////		);
////		if (tmpl.templated == "BUILTIN") {
////			if (tmpl.name == "print" || tmpl.name == "println") {
////				std::stringstream ss;
////
////				ss << "(Void)(std::cout";
////				for (const auto& [_, arg] : arguments) {
////					auto expr_s = expr_to_printable(state, arg);
////					return_if_error(expr_s);
////					ss << " << " << expr_s.value();
////				}
////				bool newline = tmpl.name == "println";
////				if (newline)
////					ss << " << \"\\n\"";
////				ss << ")";
////				return expression_information{ non_type_information{
////					.type = *state.state.named.types.at("Void").back(),
////					.representation = ss.str(),
////					.value_category = NodeStructs::Value{}
////				} };
////			}
////			throw;
////		}
////		throw;
////	}
////	if (std::holds_alternative<NodeStructs::FunctionType>(operand_t.value().type)) {
////		const auto& fn = std::get<NodeStructs::FunctionType>(operand_t.value().type).function.get();
////		if (!state.state.traversed_functions.contains(fn)) {
////			state.state.traversed_functions.insert(fn);
////			auto t = transpile(state.unindented(), fn);
////			return_if_error(t);
////			state.state.transpile_in_reverse_order.push_back(std::move(t).value());
////		}
////		auto args_or_error = transpile_args(state, arguments);
////		return_if_error(args_or_error);
////		auto ret = type_of_typename(state, fn.returnType);
////		return_if_error(ret);
////		auto f_repr = transpile_typename(state, namespaced_function);
////		return_if_error(f_repr);
////		return expression_information{ non_type_information{
////			.type = ret.value(),
////			.representation = f_repr.value() + "(" + std::move(args_or_error).value() + ")",
////			.value_category = NodeStructs::Value{}
////		} };
////	}
////	auto t = type_of_function_like_call_with_args(state, arguments, operand_t.value());
////	return_if_error(t);
////	auto args_or_error = transpile_args(state, arguments);
////	return_if_error(args_or_error);
////	auto f_repr = transpile_typename(state, namespaced_function);
////	return_if_error(f_repr);
////	return expression_information{ non_type_information{
////		.type = t.value().second,
////		.representation =f_repr.value() + "(" + std::move(args_or_error).value() + ")",
////		.value_category = NodeStructs::Value{}
////	} };
////}
//
//R T::operator()(const NodeStructs::ConditionalExpression& expr) {
//	return base(state, arguments, expr);
//}
//
//R T::operator()(const NodeStructs::OrExpression& expr) {
//	return base(state, arguments, expr);
//}
//
//R T::operator()(const NodeStructs::AndExpression& expr) {
//	return base(state, arguments, expr);
//}
//
//R T::operator()(const NodeStructs::EqualityExpression& expr) {
//	return base(state, arguments, expr);
//}
//
//R T::operator()(const NodeStructs::CompareExpression& expr) {
//	return base(state, arguments, expr);
//}
//
//R T::operator()(const NodeStructs::AdditiveExpression& expr) {
//	return base(state, arguments, expr);
//}
//
//R T::operator()(const NodeStructs::MultiplicativeExpression& expr) {
//	return base(state, arguments, expr);
//}
//
//R T::operator()(const NodeStructs::UnaryExpression& expr) {
//	return base(state, arguments, expr);
//}
//
//R T::operator()(const NodeStructs::CallExpression& expr) {
//	return base(state, arguments, expr);
//}
//
//R T::operator()(const NodeStructs::NamespaceExpression& expr) {
//	throw;
//}
//
//R T::operator()(const NodeStructs::TemplateExpression& expr) {
//	return base(state, arguments, expr);
//}
//
//R T::operator()(const NodeStructs::ConstructExpression& expr) {
//	return base(state, arguments, expr);
//}
//
//R T::operator()(const NodeStructs::BracketAccessExpression& expr) {
//	return base(state, arguments, expr);
//}
//
//R T::operator()(const NodeStructs::PropertyAccessAndCallExpression& expr) {
//	throw;
//}
//
//R T::operator()(const NodeStructs::PropertyAccessExpression& expr) {
//	auto operand_t = transpile_expression(state, expr.operand);
//	return_if_error(operand_t);
//	if (!std::holds_alternative<non_type_information>(operand_t.value()))
//		throw;
//	const auto& operand_t_ok = std::get<non_type_information>(operand_t.value());
//	auto t = type_of_postfix_member(state, expr.property_name, operand_t_ok.type.type);
//	return_if_error(t);
//
//	throw;
//	/*if (std::holds_alternative<NodeStructs::BuiltInType>(t.value().second.value)) {
//		const auto& builtin = std::get<NodeStructs::BuiltInType>(t.value().second.value);
//		return std::visit(
//			overload(
//				[&](const NodeStructs::BuiltInType::push_t& e) -> R {
//					if (arguments.size() != 1)
//						throw;
//					auto arg_t = transpile_expression(state, arguments.at(0).expr);
//					return_if_error(arg_t);
//					if (!std::holds_alternative<non_primitive_information>(arg_t.value()))
//						throw;
//					const auto& arg_t_ok = std::get<non_primitive_information>(arg_t.value());
//					if (!is_assignable_to(state, e.container.value_type.get(), { NodeStructs::TypeType{ arg_t_ok.type } })) {
//						throw;
//					}
//
//					auto operand_repr = transpile_expression(state, expr.operand);
//					return_if_error(operand_repr);
//					if (!std::holds_alternative<non_primitive_information>(operand_repr.value()))
//						throw;
//					const auto& operand_repr_ok = std::get<non_primitive_information>(operand_repr.value());
//
//					auto args_repr = transpile_args(state, arguments);
//					return_if_error(args_repr);
//					return "push(" + operand_repr_ok.representation + ", " + args_repr.value() + ")";
//				}
//			),
//			builtin.builtin
//		);
//	}
//
//	if (std::holds_alternative<NodeStructs::FunctionType>(t.value().second.value)) {
//		const auto& fn = std::get<NodeStructs::FunctionType>(t.value().second.value);
//		const auto& unwrapped_fn = fn.function.get();
//	}
//
//	return error{
//		"user error",
//		"Use of type like a function is prohibited. Type was `" + transpile_type(state, t.value().second).value() + "`"
//	};*/
//}
//
//R T::operator()(const NodeStructs::ParenArguments& expr) {
//	return base(state, arguments, expr);
//}
//
//R T::operator()(const NodeStructs::BraceArguments& expr) {
//	return base(state, arguments, expr);
//}
//
//R T::operator()(const std::string& expr) {
//	return base(state, arguments, expr);
//}
//
//R T::operator()(const Token<INTEGER_NUMBER>& expr) {
//	throw;
//}
//
//R T::operator()(const Token<FLOATING_POINT_NUMBER>& expr) {
//	throw;
//}
//
//R T::operator()(const Token<STRING>& expr) {
//	throw;
//}
