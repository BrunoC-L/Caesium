#include "../core/toCPP.hpp"
//#include "transpile_call_expression_with_args.hpp"
#include "../utility/vec_of_expected_to_expected_of_vec.hpp"

using T = transpile_call_expression_with_args_visitor;
using R = T::R;

R base(
	transpilation_state_with_indent state,
	const std::vector<NodeStructs::FunctionArgument>& arguments,
	const auto& expr
) {
	auto operand_t = transpile_expression(state, expr).transform([](auto&& x) { return std::pair{ std::move(x).value_category, std::move(x).type }; });
	return_if_error(operand_t);

	if (std::holds_alternative<NodeStructs::Template>(operand_t.value().second.value)) {
		auto arg_ts = vec_of_expected_to_expected_of_vec(
			arguments
			| LIFT_TRANSFORM(std::get<NodeStructs::Expression>)
			| LIFT_TRANSFORM_X(X, transpile_expression(state, X).transform([](auto&& x) { return std::pair{ std::move(x).value_category, std::move(x).type }; }))
			| to_vec()
		);
		const auto& tmpl = std::get<NodeStructs::Template>(operand_t.value().second.value);
		if (tmpl.templated == "BUILTIN") {
			if (tmpl.name == "print" || tmpl.name == "println") {
				if (arguments.size() == 0)
					return "";
				std::stringstream ss;

				ss << "(Void)(std::cout";
				for (const auto& [_, arg] : arguments) {
					auto expr_s = expr_to_printable(state, arg);
					return_if_error(expr_s);
					ss << " << " << expr_s.value();
				}
				bool newline = tmpl.name == "println";
				if (newline)
					ss << " << \"\\n\"";
				ss << ")";
				return ss.str();
			}
			throw;
		}
		throw;
	}

	if (std::holds_alternative<NodeStructs::BuiltInType>(operand_t.value().second.value)) {
		std::visit(
			overload(
				[&](const NodeStructs::BuiltInType::push_t& e) {
					throw;
				}
			),
			std::get<NodeStructs::BuiltInType>(operand_t.value().second.value).builtin
		);
		throw;
	}

	auto t = type_of_function_like_call_with_args(state, arguments, operand_t.value().second);
	return_if_error(t);
	auto args_or_error = transpile_args(state, arguments);
	return_if_error(args_or_error);
	auto operand_repr = transpile_expression(state, expr);
	return_if_error(operand_repr);
	return std::move(operand_repr).value().representation + "(" + std::move(args_or_error).value() + ")";
}

R T::operator()(const NodeStructs::ConditionalExpression& expr) {
	return base(state, arguments, expr);
}

R T::operator()(const NodeStructs::OrExpression& expr) {
	return base(state, arguments, expr);
}

R T::operator()(const NodeStructs::AndExpression& expr) {
	return base(state, arguments, expr);
}

R T::operator()(const NodeStructs::EqualityExpression& expr) {
	return base(state, arguments, expr);
}

R T::operator()(const NodeStructs::CompareExpression& expr) {
	return base(state, arguments, expr);
}

R T::operator()(const NodeStructs::AdditiveExpression& expr) {
	return base(state, arguments, expr);
}

R T::operator()(const NodeStructs::MultiplicativeExpression& expr) {
	return base(state, arguments, expr);
}

R T::operator()(const NodeStructs::UnaryExpression& expr) {
	return base(state, arguments, expr);
}

R T::operator()(const NodeStructs::CallExpression& expr) {
	return base(state, arguments, expr);
}

R T::operator()(const NodeStructs::TemplateExpression& expr) {
	return base(state, arguments, expr);
}

R T::operator()(const NodeStructs::ConstructExpression& expr) {
	return base(state, arguments, expr);
}

R T::operator()(const NodeStructs::BracketAccessExpression& expr) {
	return base(state, arguments, expr);
}

R T::operator()(const NodeStructs::PropertyAccessAndCallExpression& expr) {
	throw;
}

R T::operator()(const NodeStructs::PropertyAccessExpression& expr) {
	auto operand_t = transpile_expression(state, expr.operand).transform([](auto&& x) { return std::pair{ std::move(x).value_category, std::move(x).type }; });
	return_if_error(operand_t);
	auto t = type_of_postfix_member(state, expr.property_name, operand_t.value().second);
	return_if_error(t);

	if (std::holds_alternative<NodeStructs::BuiltInType>(t.value().second.value)) {
		const auto& builtin = std::get<NodeStructs::BuiltInType>(t.value().second.value);
		return std::visit(
			overload(
				[&](const NodeStructs::BuiltInType::push_t& e) -> R {
					if (arguments.size() != 1)
						throw;
					auto arg_t = transpile_expression(state, std::get<NodeStructs::Expression>(arguments.at(0))).transform([](auto&& x) { return std::pair{ std::move(x).value_category, std::move(x).type }; });
					return_if_error(arg_t);
					if (!is_assignable_to(state, e.container.value_type.get(), arg_t.value().second)) {
						throw;
					}

					auto operand_repr = transpile_expression(state, expr.operand);
					return_if_error(operand_repr);
					auto args_repr = transpile_args(state, arguments);
					return_if_error(args_repr);
					return "push(" + operand_repr.value().representation + ", " + args_repr.value() + ")";
				}
			),
			builtin.builtin
		);
	}

	if (std::holds_alternative<NodeStructs::FunctionType>(t.value().second.value)) {
		const auto& fn = std::get<NodeStructs::FunctionType>(t.value().second.value);
		const auto& unwrapped_fn = fn.function.get();
	}

	return error{
		"user error",
		"Use of type like a function is prohibited. Type was `" + transpile_type(state, t.value().second).value() + "`"
	};
}

R T::operator()(const NodeStructs::ParenArguments& expr) {
	return base(state, arguments, expr);
}

R T::operator()(const NodeStructs::BraceArguments& expr) {
	return base(state, arguments, expr);
}

R T::operator()(const std::string& expr) {
	return base(state, arguments, expr);
}

R T::operator()(const Token<INTEGER_NUMBER>& expr) {
	throw;
}

R T::operator()(const Token<FLOATING_POINT_NUMBER>& expr) {
	throw;
}

R T::operator()(const Token<STRING>& expr) {
	throw;
}
