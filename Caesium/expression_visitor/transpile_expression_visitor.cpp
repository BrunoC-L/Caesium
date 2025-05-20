#include "../core/toCPP.hpp"
#include "../utility/replace_all.hpp"
#include "../structured/structurizer.hpp"
#include "../utility/vec_of_expected_to_expected_of_vec.hpp"
#include "../utility/vec_of_variant_to_optional_vector_single_type.hpp"
#include <iostream>
#include <algorithm>

using T = transpile_expression_visitor;
using R = T::R;

bool is_int(const Realised::MetaType& t) {
	return holds<Realised::PrimitiveType>(t)
		&& (holds<Realised::PrimitiveType::Valued<int>>(get<Realised::PrimitiveType>(t).value)
			|| holds<Realised::PrimitiveType::NonValued<int>>(get<Realised::PrimitiveType>(t).value));
}

bool is_bool(const Realised::MetaType& t) {
	return holds<Realised::PrimitiveType>(t)
		&& (holds<Realised::PrimitiveType::Valued<bool>>(get<Realised::PrimitiveType>(t).value)
			|| holds<Realised::PrimitiveType::NonValued<bool>>(get<Realised::PrimitiveType>(t).value));
}

bool is_floating(const Realised::MetaType& t) {
	return holds<Realised::PrimitiveType>(t)
		&& (holds<Realised::PrimitiveType::Valued<double>>(get<Realised::PrimitiveType>(t).value)
			|| holds<Realised::PrimitiveType::NonValued<double>>(get<Realised::PrimitiveType>(t).value));
}

bool is_char_or_string(const Realised::MetaType& t) {
	return holds<Realised::PrimitiveType>(t)
		&& (holds<Realised::PrimitiveType::Valued<std::string>>(get<Realised::PrimitiveType>(t).value)
			|| holds<Realised::PrimitiveType::NonValued<std::string>>(get<Realised::PrimitiveType>(t).value)
			|| holds<Realised::PrimitiveType::Valued<char>>(get<Realised::PrimitiveType>(t).value)
			|| holds<Realised::PrimitiveType::NonValued<char>>(get<Realised::PrimitiveType>(t).value));
}

R T::operator()(const NodeStructs::ConditionalExpression& expr) {
	if (expr.ifElseExprs.has_value()) {
		// x if first else second
		//                  ([&] () { if (first) return x; else return second; }());

		auto condition_expr_info = operator()(expr.ifElseExprs.value().first);
		return_if_error(condition_expr_info);
		if (!std::holds_alternative<non_type_information>(condition_expr_info.value()))
			NOT_IMPLEMENTED;
		const non_type_information& condition_expr_info_ok = std::get<non_type_information>(condition_expr_info.value());

		auto if_expr_info = operator()(expr.expr);
		return_if_error(if_expr_info);
		if (!std::holds_alternative<non_type_information>(if_expr_info.value()))
			NOT_IMPLEMENTED;
		non_type_information& if_expr_info_ok = std::get<non_type_information>(condition_expr_info.value());
		if (cmp(if_expr_info_ok.type, Realised::MetaType{ Realised::PrimitiveType{ Realised::PrimitiveType::NonValued<bool>{} } }) != std::strong_ordering::equivalent)
		NOT_IMPLEMENTED;

		auto else_expr_info = operator()(expr.ifElseExprs.value().second);
		return_if_error(else_expr_info);
		if (!std::holds_alternative<non_type_information>(else_expr_info.value()))
			NOT_IMPLEMENTED;
		non_type_information& else_expr_info_ok = std::get<non_type_information>(condition_expr_info.value());

		return expression_information{ non_type_information{
			.type = std::move(if_expr_info_ok.type),
			.representation = std::string("([&] () { if (") +
			condition_expr_info_ok.representation +
			") return " +
			if_expr_info_ok.representation +
			"; else return " +
			else_expr_info_ok.representation +
			"; }())",
			.value_category = std::move(if_expr_info_ok.value_category),
		} };
	}
	else
		return operator()(expr.expr);
}

R T::operator()(const NodeStructs::OrExpression& expr) {
	std::stringstream ss;
	auto base = operator()(expr.expr);
	return_if_error(base);
	if (!std::holds_alternative<non_type_information>(base.value()))
		NOT_IMPLEMENTED;
	const non_type_information& base_ok = std::get<non_type_information>(base.value());
	if (cmp(base_ok.type, Realised::MetaType{ Realised::PrimitiveType{ Realised::PrimitiveType::NonValued<bool>{} } }) != std::strong_ordering::equivalent)
	NOT_IMPLEMENTED;
	ss << base_ok.representation;
	for (const auto& e : expr.ors) {
		auto or_expr = operator()(e);
		return_if_error(or_expr);
		if (!std::holds_alternative<non_type_information>(or_expr.value()))
			NOT_IMPLEMENTED;
		const non_type_information& or_expr_ok = std::get<non_type_information>(or_expr.value());
		if (cmp(or_expr_ok.type, Realised::MetaType{ Realised::PrimitiveType{ Realised::PrimitiveType::NonValued<bool>{} } }) != std::strong_ordering::equivalent)
		NOT_IMPLEMENTED;
		ss << " || " << or_expr_ok.representation;
	}
	return expression_information{ non_type_information{
		.type = { Realised::PrimitiveType{ Realised::PrimitiveType::NonValued<bool>{} } },
		.representation = ss.str(),
		.value_category = NodeStructs::Value{},
	} };
}

R T::operator()(const NodeStructs::AndExpression& expr) {
	std::stringstream ss;
	auto base = operator()(expr.expr);
	return_if_error(base);
	if (!std::holds_alternative<non_type_information>(base.value()))
		NOT_IMPLEMENTED;
	const non_type_information& base_ok = std::get<non_type_information>(base.value());
	if (cmp(base_ok.type, Realised::MetaType{ Realised::PrimitiveType{ Realised::PrimitiveType::NonValued<bool>{} } }) != std::strong_ordering::equivalent)
		NOT_IMPLEMENTED;
	ss << base_ok.representation;
	for (const auto& e : expr.ands) {
		auto and_ = operator()(e);
		return_if_error(and_);
		if (!std::holds_alternative<non_type_information>(and_.value()))
			NOT_IMPLEMENTED;
		const non_type_information& and_ok = std::get<non_type_information>(and_.value());
		if (cmp(and_ok.type, Realised::MetaType{ Realised::PrimitiveType{ Realised::PrimitiveType::NonValued<bool>{} } }) != std::strong_ordering::equivalent)
			NOT_IMPLEMENTED;
		ss << " || " << and_ok.representation;
	}
	return expression_information{ non_type_information{
		.type = { Realised::PrimitiveType{ Realised::PrimitiveType::NonValued<bool>{} } },
		.representation = ss.str(),
		.value_category = NodeStructs::Value{},
	} };
}

R T::operator()(const NodeStructs::EqualityExpression& expr) {
	if (expr.equals.size() != 1)
		NOT_IMPLEMENTED;
	auto base = operator()(expr.expr);
	return_if_error(base);
	if (std::holds_alternative<non_type_information>(base.value())) {
		std::stringstream ss;
		const non_type_information& base_ok = std::get<non_type_information>(base.value());
		if (!holds<Realised::PrimitiveType>(base_ok.type) && !holds<Realised::EnumType>(base_ok.type))
			NOT_IMPLEMENTED;
		ss << base_ok.representation;
		for (const auto& [op, e] : expr.equals) {
			auto eq = operator()(e);
			return_if_error(eq);
			if (!std::holds_alternative<non_type_information>(eq.value()))
				NOT_IMPLEMENTED;
			const non_type_information& eq_ok = std::get<non_type_information>(eq.value());

			if (std::holds_alternative<directly_assignable>(assigned_to(state, variables, base_ok.type, eq_ok.type)._value))
				ss << " " << symbol_variant_as_text(op._value) << " " << eq_ok.representation;
			else
				return error{
					"user error",
					"cannot compare types for EqualityExpression, left was `"
					+ original_representation(expr.expr)
					+ "`, right was `"
					+ original_representation(e)
					+ "`"
				};
		}
		return expression_information{ non_type_information{
			.type = { Realised::PrimitiveType{ Realised::PrimitiveType::NonValued<bool>{} } },
			.representation = ss.str(),
			.value_category = NodeStructs::Value{},
		} };
	}
	else {
		const type_information& left = std::get<type_information>(base.value());
		if (expr.equals.size() != 1)
			NOT_IMPLEMENTED;
		const auto& [op, e] = expr.equals[0];
		auto right = operator()(e);
		return_if_error(right);
		if (!std::holds_alternative<type_information>(right.value()))
			NOT_IMPLEMENTED;
		const type_information& right_ok = std::get<type_information>(right.value());
		auto [str, val] = [&]() -> std::pair<std::string, bool> {
			if (cmp(left.type, right_ok.type) == std::strong_ordering::equivalent)
				return { "True", true };
			else
				return { "False", false };
		}();
		return expression_information{ non_type_information{
			.type = { Realised::PrimitiveType{ Realised::PrimitiveType::Valued<bool>{ val } } },
			.representation = std::move(str),
			.value_category = NodeStructs::Value{},
		} };
	}
}

R T::operator()(const NodeStructs::CompareExpression& expr) {
	std::stringstream ss;
	auto base = operator()(expr.expr);
	return_if_error(base);
	if (!std::holds_alternative<non_type_information>(base.value()))
		NOT_IMPLEMENTED;
	const non_type_information& base_ok = std::get<non_type_information>(base.value());
	if (!is_int(base_ok.type) && !is_floating(base_ok.type) && !is_char_or_string(base_ok.type))
		NOT_IMPLEMENTED;
	ss << base_ok.representation;
	if (expr.comparisons.size() != 1)
		NOT_IMPLEMENTED; // todo implement a > b > c, which is not (a>b)>c but rather (a>b)&&(b>c)
	for (const auto& [op, e] : expr.comparisons) {
		auto cmp_ = operator()(e);
		return_if_error(cmp_);
		if (!std::holds_alternative<non_type_information>(cmp_.value()))
			NOT_IMPLEMENTED;
		const non_type_information& cmp_ok = std::get<non_type_information>(cmp_.value());
		if (!holds<Realised::PrimitiveType>(cmp_ok.type))
			NOT_IMPLEMENTED;
		if (get<Realised::PrimitiveType>(base_ok.type).value._value.index() == get<Realised::PrimitiveType>(cmp_ok.type).value._value.index()
			|| get<Realised::PrimitiveType>(base_ok.type).value._value.index() + 7 == get<Realised::PrimitiveType>(cmp_ok.type).value._value.index()
			|| get<Realised::PrimitiveType>(base_ok.type).value._value.index() - 7 == get<Realised::PrimitiveType>(cmp_ok.type).value._value.index())
				ss << " " << symbol_variant_as_text(op._value) << " " << cmp_ok.representation;
		else
			return error{ "user error", "cannot compare types for CompareExpression" };
	}
	return expression_information{ non_type_information{
		.type = { Realised::PrimitiveType{ Realised::PrimitiveType::NonValued<bool>{} } },
		.representation = ss.str(),
		.value_category = NodeStructs::Value{},
	} };
}

template <typename OP, typename E>
static R sum_numbers(auto& vis, const non_type_information& base, const std::vector<std::pair<OP, E>>& v, bool has_floating) {
	std::stringstream ss;
	ss << "(" << base.representation;
	for (const auto& [op, e] : v) {
		auto add = vis(e);
		return_if_error(add);
		if (!std::holds_alternative<non_type_information>(add.value()))
			NOT_IMPLEMENTED;
		const non_type_information& add_ok = std::get<non_type_information>(add.value());
		if (is_int(add_ok.type))
			ss << " " << symbol_variant_as_text(op._value) << " " << add_ok.representation;
		else if (is_floating(add_ok.type)) {
			has_floating = true;
			ss << " " << symbol_variant_as_text(op._value) << " " << add_ok.representation;
		}
		else
			return error{ "user error", "Expected an Integer or Floating to sum with previous Integer or Floating" };
	}
	ss << ")";
	return expression_information{ non_type_information{
		.type = Realised::MetaType{
			has_floating ?
			Realised::MetaType{ Realised::PrimitiveType{ Realised::PrimitiveType::NonValued<double>{} } }
			: Realised::MetaType{ Realised::PrimitiveType{ Realised::PrimitiveType::NonValued<int>{} } }
		},
		.representation = ss.str(),
		.value_category = NodeStructs::Value{},
	} };
}

template <typename OP, typename E>
static R sum_strings(auto& vis, const non_type_information& base, const std::vector<std::pair<OP, E>>& v) {
	std::stringstream ss;
	ss << "sum_strings(" << base.representation;
	for (const auto& [op, e] : v) {
		auto add = vis(e);
		return_if_error(add);
		if (!std::holds_alternative<non_type_information>(add.value()))
			NOT_IMPLEMENTED;
		const non_type_information& add_ok = std::get<non_type_information>(add.value());
		if (!std::holds_alternative<Token<PLUS>>(op._value))
			return error{ "user error", "Addition between Char and String only allows for +, not -" };
		if (is_char_or_string(add_ok.type))
			ss << ", " << add_ok.representation;
		else
			return error{ "user error", "Expected a Char or String to sum with previous Char or String" };
	}
	ss << ")";
	return expression_information{ non_type_information{
		.type = Realised::MetaType{ Realised::PrimitiveType{ Realised::PrimitiveType::NonValued<std::string>{} } },
		.representation = ss.str(),
		.value_category = NodeStructs::Value{},
	} };
}

R T::operator()(const NodeStructs::AdditiveExpression& expr) {
	std::stringstream ss;
	auto base = operator()(expr.expr);
	return_if_error(base);
	if (!std::holds_alternative<non_type_information>(base.value()))
		NOT_IMPLEMENTED;
	const non_type_information& base_ok = std::get<non_type_information>(base.value());
	if (is_int(base_ok.type))
		return sum_numbers(*this, base_ok, expr.adds, false);
	else if (is_floating(base_ok.type))
		return sum_numbers(*this, base_ok, expr.adds, true);
	else if (is_char_or_string(base_ok.type))
		return sum_strings(*this, base_ok, expr.adds);
	else
		return error{
			"user error",
			"expected Integer, Floating, Char or String for addition"
	};
}

R T::operator()(const NodeStructs::MultiplicativeExpression& expr) {
	std::stringstream ss;
	auto base = operator()(expr.expr);
	return_if_error(base);
	if (!std::holds_alternative<non_type_information>(base.value()))
		NOT_IMPLEMENTED;
	const non_type_information& base_ok = std::get<non_type_information>(base.value());
	if (!is_int(base_ok.type) && !is_floating(base_ok.type))
		return error{ "user error", "multiplication is only available for Int and Floating" };
	ss << base_ok.representation;

	for (const auto& [op, e] : expr.muls) {
		auto mul = operator()(e);
		return_if_error(mul);
		if (!std::holds_alternative<non_type_information>(mul.value()))
			NOT_IMPLEMENTED;
		const non_type_information& mul_ok = std::get<non_type_information>(mul.value());
		if (!is_int(mul_ok.type) && !is_floating(mul_ok.type))
			return error{ "user error", "multiplication is only available for Int and Floating" };
		ss << " " << symbol_variant_as_text(op._value) << " " << mul_ok.representation;
	}

	return expression_information{ non_type_information{
		.type = Realised::MetaType{ Realised::PrimitiveType{ Realised::PrimitiveType::NonValued<int>{} } },
		.representation = ss.str(),
		.value_category = NodeStructs::Value{},
	} };
}

R T::operator()(const NodeStructs::UnaryExpression& expr) {
	if (expr.unary_operators.size() != 1)
		NOT_IMPLEMENTED;

	/*for (const NodeStructs::UnaryExpression::op_types& op : expr.unary_operators)
		ss << symbol_variant_as_text(op._value);*/

	auto base = operator()(expr.expr);
	return_if_error(base);
	if (!std::holds_alternative<non_type_information>(base.value()))
		NOT_IMPLEMENTED;
	const non_type_information& base_ok = std::get<non_type_information>(base.value());

	if (holds<Realised::CompileTimeType>(base_ok.type)) {
		if (holds<Token<DASH>>(expr.unary_operators[0])) {
			if (!holds<Realised::PrimitiveType>(get<Realised::CompileTimeType>(base_ok.type).type))
				NOT_IMPLEMENTED;
			if (holds<Realised::PrimitiveType::Valued<int>>(get<Realised::PrimitiveType>(get<Realised::CompileTimeType>(base_ok.type).type).value)) {
				int val = get<Realised::PrimitiveType::Valued<int>>(get<Realised::PrimitiveType>(get<Realised::CompileTimeType>(base_ok.type).type).value).value;
				return expression_information{ non_type_information{
					.type = { Realised::PrimitiveType{ Realised::PrimitiveType::Valued<int>{ -val } } },
					.representation = std::to_string(-val),
					.value_category = NodeStructs::Value{},
				} };
			}
			if (holds<Realised::PrimitiveType::Valued<double>>(get<Realised::PrimitiveType>(get<Realised::CompileTimeType>(base_ok.type).type).value)) {
				double val = get<Realised::PrimitiveType::Valued<double>>(get<Realised::PrimitiveType>(get<Realised::CompileTimeType>(base_ok.type).type).value).value;
				return expression_information{ non_type_information{
					.type = { Realised::PrimitiveType{ Realised::PrimitiveType::Valued<double>{ -val } } },
					.representation = std::to_string(-val),
					.value_category = NodeStructs::Value{},
				} };
			}
			NOT_IMPLEMENTED;
		}
		if (holds<Token<NOT>>(expr.unary_operators[0])) {
			if (!holds<Realised::PrimitiveType>(get<Realised::CompileTimeType>(base_ok.type).type))
				NOT_IMPLEMENTED;
			if (holds<Realised::PrimitiveType::Valued<bool>>(get<Realised::PrimitiveType>(get<Realised::CompileTimeType>(base_ok.type).type).value)) {
				bool val = get<Realised::PrimitiveType::Valued<bool>>(get<Realised::PrimitiveType>(get<Realised::CompileTimeType>(base_ok.type).type).value).value;
				return expression_information{ non_type_information{
					.type = { Realised::PrimitiveType{ Realised::PrimitiveType::Valued<bool>{ !val } } },
					.representation = std::to_string(!val),
					.value_category = NodeStructs::Value{},
				} };
			}
			NOT_IMPLEMENTED;
		}
		NOT_IMPLEMENTED;
	}
	else {
		if (holds<Token<DASH>>(expr.unary_operators[0]) && !(is_int(base_ok.type) || is_floating(base_ok.type)))
			NOT_IMPLEMENTED;
		if (holds<Token<NOT>>(expr.unary_operators[0]) && !is_bool(base_ok.type))
			NOT_IMPLEMENTED;
		std::stringstream ss;
		ss << symbol_variant_as_text(expr.unary_operators[0]._value) << base_ok.representation;
		return expression_information{ non_type_information{
			.type = copy(base_ok.type),
			.representation = ss.str(),
			.value_category = NodeStructs::Value{},
		} };
	}

}

R T::operator()(const NodeStructs::CallExpression& expr) {
	if (holds<std::string>(expr.operand)) {
		NOT_IMPLEMENTED;
		/*const std::string& operand = get<std::string>(expr.operand);
		if (auto it = state.state.global_namespace.functions_using_auto.find(operand); it != state.state.global_namespace.functions_using_auto.end()) {
			auto args_ = vec_of_expected_to_expected_of_vec(expr.arguments.args
				| std::views::transform([&](auto&& arg) { return transpile_arg(state, variables, arg); })
				| to_vec());
			return_if_error(args_);
			auto args_ok_maybe_wrong_type = std::move(args_).value();
			auto opt = vec_of_variant_to_optional_vector_single_type<non_type_information>(std::move(args_ok_maybe_wrong_type));
			if (!opt.has_value())
				NOT_IMPLEMENTED;
			const auto& args_ok = opt.value();
			auto fn = realise_function_using_auto(
				state,
				it->second.back(),
				args_ok | std::views::transform([](const auto& e) { return copy(e.type); }) | to_vec()
			);
			return_if_error(fn);
			auto& vec = state.state.global_namespace.functions[fn.value().name];
			bool found = false;
			for (const auto& f : vec)
				if (cmp(f, fn.value()) == std::strong_ordering::equivalent)
					found = true;
			if (!found)
				vec.push_back(std::move(fn).value());
		}*/
	}
	auto t = operator()(expr.operand);
	return_if_error(t);
	if (std::holds_alternative<type_information>(t.value())) {
		const auto& ti = std::get<type_information>(t.value());
		if (holds<Realised::Builtin>(ti.type)) {
			NOT_IMPLEMENTED;
			/*const std::string& name = get<Realised::Builtin>(ti.type).name;
			if (name == "print" || name == "println") {
				std::stringstream ss;

				ss << "(Void)(std::cout";
				for (const auto& [_, arg] : expr.arguments.args) {
					auto expr_s = expr_to_printable(state, variables, arg);
					return_if_error(expr_s);
					ss << " << " << expr_s.value();
				}
				bool newline = name == "println";
				if (newline)
					ss << " << \"\\n\"";
				ss << ")";
				return expression_information{ non_type_information{
					.type = Realised::MetaType{ Realised::PrimitiveType{ Realised::PrimitiveType::NonValued<Realised::void_t>{} } },
					.representation = ss.str(),
					.value_category = NodeStructs::Value{}
				} };
			}
			if (name == "exit") {
				std::stringstream ss;
				ss << "exit(";
				if (expr.arguments.args.size() != 1)
					NOT_IMPLEMENTED;
				auto expr_info = operator()(expr.arguments.args.at(0).expr);
				return_if_error(expr_info);
				if (!std::holds_alternative<non_type_information>(expr_info.value()))
					NOT_IMPLEMENTED;
				const non_type_information& expr_info_ok = std::get<non_type_information>(expr_info.value());
				if (!std::holds_alternative<directly_assignable>(assigned_to(state, variables, Realised::MetaType{ Realised::PrimitiveType{ Realised::PrimitiveType::NonValued<int>{} } }, expr_info_ok.type)._value))
					NOT_IMPLEMENTED;
				ss << expr_info_ok.representation << ")";
				return expression_information{ non_type_information{
					.type = Realised::MetaType{ Realised::PrimitiveType{ Realised::PrimitiveType::NonValued<Realised::void_t>{} } },
					.representation = ss.str(),
					.value_category = NodeStructs::Value{}
				} };
			}
			if (name == "compile_time_error") {
				return error{
					"user error",
					"user activated compile_time_error encountered"
				};
			}*/
		}
		NOT_IMPLEMENTED;
		//if (holds<Realised::TemplateType>(ti.type)) {
		//	const auto& options = get<Realised::TemplateType>(ti.type)
		//		.name_space.get()
		//		.templates.at(get<Realised::TemplateType>(ti.type).name);
		//	if (options.size() != 1)
		//		NOT_IMPLEMENTED;
		//	// const auto& tmpl = options.back();
		//	/*auto arg_ts = vec_of_expected_to_expected_of_vec(
		//		expr.arguments.args
		//		| std::views::transform([&](auto&& e) { return e.expr; })
		//		| std::views::transform([&](auto&& e) { return transpile_expression(state, e); })
		//		| to_vec()
		//	);*/
		//	NOT_IMPLEMENTED;
		//}
		if (holds<Realised::FunctionType>(ti.type)) {
			auto args_ = vec_of_expected_to_expected_of_vec(expr.arguments.args
				| std::views::transform([&](auto&& arg) { return transpile_arg(state, variables, arg); })
				| to_vec());
			return_if_error(args_);
			auto args_ok_maybe_wrong_type = std::move(args_).value();
			auto opt = vec_of_variant_to_optional_vector_single_type<non_type_information>(std::move(args_ok_maybe_wrong_type));
			if (!opt.has_value())
				NOT_IMPLEMENTED;
			const auto& args_ok = opt.value();
			NOT_IMPLEMENTED;
			/*auto expected_f = find_best_function(
				state,
				variables,
				get<Realised::FunctionType>(ti.type).name,
				get<Realised::FunctionType>(ti.type).name_space,
				args_ok | std::views::transform([](const auto& e) { return copy(e.type); }) | to_vec()
			);
			return_if_error(expected_f);
			if (!expected_f.value().has_value())
				return error{ "user error", "no matching function" };
			const NodeStructs::Function& fn = expected_f.value().value().get();

			if (!state.state.functions_traversal.traversed.count(fn.name)) {
				state.state.functions_traversal.traversing.insert(fn.name);
				auto transpiled_f = transpile(state.unindented(), fn);
				return_if_error(transpiled_f);
				if (uses_auto(fn))
					NOT_IMPLEMENTED;
				state.state.functions_traversal.traversing.erase(fn.name);
				state.state.functions_traversal.traversed.insert({ fn.name, copy(fn) });
			}
			auto args_repr = [&]() {
				std::stringstream ss;
				bool has_prev = false;
				for (const auto& arg : args_ok) {
					if (has_prev)
						ss << ", ";
					else
						has_prev = true;
					ss << arg.representation;
				}
				return ss.str();
			}();
			return expression_information{ non_type_information{
				.type = type_of_typename(state, variables, fn.returnType).value(),
				.representation = ti.representation + "(" + std::move(args_repr) + ")",
				.value_category = NodeStructs::Value{}
			} };*/
		}
		NOT_IMPLEMENTED;
	}
	NOT_IMPLEMENTED;
}

R T::operator()(const NodeStructs::NamespaceExpression& expr) {
	transpile_expression_information_t operand_t = operator()(expr.name_space);
	return_if_error(operand_t);
	if (!std::holds_alternative<type_information>(operand_t.value()))
		NOT_IMPLEMENTED;
	const auto& operand_t_ok = std::get<type_information>(operand_t.value());
	NOT_IMPLEMENTED;
	//return std::visit(overload(
	//	[&](const NodeStructs::NamespaceType& nst) -> transpile_expression_information_t {
	//		const auto& ns = nst.name_space.get();
	//		if (auto it = ns.types.find(expr.name_in_name_space); it != ns.types.end()) {
	//			NOT_IMPLEMENTED;
	//			/*return expression_information{ type_information{
	//				.type = { copy(it->second.back()) },
	//				.representation = operand_t_ok.representation + "__" + expr.name_in_name_space
	//			} };*/
	//		}
	//		if (auto it = ns.functions.find(expr.name_in_name_space); it != ns.functions.end())
	//			return expression_information{ type_information{
	//				.type = Realised::MetaType{ NodeStructs::FunctionType{
	//					.name = expr.name_in_name_space,
	//					.name_space = ns
	//				} },
	//				.representation = operand_t_ok.representation + "__" + expr.name_in_name_space
	//			} };
	//		if (auto it = ns.templates.find(expr.name_in_name_space); it != ns.templates.end())
	//			return expression_information{ type_information{
	//				.type = Realised::MetaType{ NodeStructs::TemplateType{
	//					.name = expr.name_in_name_space,
	//					.name_space = ns
	//				} },
	//				.representation = operand_t_ok.representation + "__" + expr.name_in_name_space
	//			} };
	//		return error{
	//			"user error",
	//			"namespace `" + ns.name + "` has no type or function `" + expr.name_in_name_space + "`"
	//		};
	//	},
	//	[&](const NodeStructs::EnumType& enumt) -> transpile_expression_information_t {
	//		const auto& enum_ = enumt.enum_.get();
	//		if (auto it = std::find(enum_.values.begin(), enum_.values.end(), expr.name_in_name_space); it != enum_.values.end()) {
	//			return expression_information{ non_type_information{
	//				.type = { NodeStructs::EnumValueType{ enum_, expr.name_in_name_space } },
	//				.representation = operand_t_ok.representation + "__" + expr.name_in_name_space,
	//				.value_category = NodeStructs::Value{}
	//			} };
	//		}
	//		return error{
	//			"user error",
	//			"enum `" + operand_t_ok.representation + "` has no member `" + expr.name_in_name_space + "`"
	//		};
	//	},
	//	[&](const auto& nst) -> transpile_expression_information_t {
	//		NOT_IMPLEMENTED;
	//	}
	//), operand_t_ok.type.type.get()._value);
}

R T::operator()(const NodeStructs::TemplateExpression& expr) {
	if (!holds<std::string>(expr.operand))
		NOT_IMPLEMENTED;
	const auto& operand = get<std::string>(expr.operand);

	NOT_IMPLEMENTED;
	//if (auto it = state.state.global_namespace.templates.find(operand);
	//	it != state.state.global_namespace.templates.end()) {
	//	NOT_IMPLEMENTED;
	//	//auto t = find_best_template(state, variables, it->second, expr.args);
	//	//return_if_error(t);
	//	//const auto& tmpl = t.value().tmpl.get();
	//	//expected<std::vector<std::string>> args = vec_of_expected_to_expected_of_vec(expr.args
	//	//	| std::views::transform([&](auto&& e) { return word_typename_or_expression_for_template(state, variables, e); })
	//	//	| to_vec());
	//	//return_if_error(args);
	//	//auto tmpl_name_or_e = word_typename_or_expression_for_template(
	//	//	state,
	//	//	variables,
	//	//	{ NodeStructs::Expression{ .expression = copy(expr), .info = copy(t.value().tmpl.get().info) }}
	//	//);
	//	//return_if_error(tmpl_name_or_e);
	//	//const auto& tmpl_name = tmpl_name_or_e.value();

	//	//size_t max_params = it->second.at(0).parameters.size();
	//	//for (unsigned i = 1; i < it->second.size(); ++i)
	//	//	max_params = std::max(max_params, it->second.at(i).parameters.size());

	//	//const auto& arg_placements = t.value().arg_placements;
	//	//auto grab_nth_with_commas = [&](size_t i) {
	//	//	std::stringstream ss;
	//	//	bool has_previous = false;
	//	//	for (size_t j = 0; j < arg_placements.size(); ++j) {
	//	//		size_t k = arg_placements.at(j);
	//	//		if (k == i) {
	//	//			const auto& arg = args.value().at(j);
	//	//			if (has_previous)
	//	//				ss << ", ";
	//	//			has_previous = true;
	//	//			ss << arg;
	//	//		}
	//	//	}
	//	//	return ss.str();
	//	//};
	//	//std::string replaced = tmpl.templated;
	//	//for (size_t i = 0; i < tmpl.parameters.size(); ++i) {
	//	//	replaced = replace_all(
	//	//		std::move(replaced),
	//	//		std::visit(overload(
	//	//			[](const auto& e) { return "`" + e.name + "`"; },
	//	//			[](const NodeStructs::VariadicTemplateParameter& e) { return "`" + e.name + "...`"; }
	//	//		), tmpl.parameters.at(i)._value),
	//	//		grab_nth_with_commas(i)
	//	//	);
	//	//}

	//	//bool has_f = state.state.global_namespace.functions.find(tmpl_name) != state.state.global_namespace.functions.end();
	//	//NOT_IMPLEMENTED;
	//	///*if (has_f)
	//	//	return expression_information{ type_information{
	//	//		.type = Realised::MetaType{ Realised::FunctionType{
	//	//			tmpl_name, state.state.global_namespace
	//	//		} },
	//	//		.representation = tmpl_name
	//	//	} };*/

	//	//if (auto it = state.state.global_namespace.types.find(tmpl_name); it != state.state.global_namespace.types.end()) {
	//	//	NOT_IMPLEMENTED;
	//	//	/*return expression_information{ type_information{
	//	//		.type = Realised::MetaType{ copy(it->second.back()) },
	//	//		.representation = tmpl_name
	//	//	} };*/
	//	//}

	//	//{
	//	//	And<IndentToken, grammar::Function, Token<END>> f{ tmpl.indent };
	//	//	auto tokens = Tokenizer(replaced).read();
	//	//	Iterator it = { .vec = tokens, .index = 0 , .line = 0, .col = 0, .file_name = "template:/" + tmpl_name };
	//	//	try {
	//	//		if (build(f, it)) {
	//	//			auto structured_f = getStruct("template:/" + tmpl_name, tokens, f.template get<grammar::Function>(), std::nullopt);
	//	//			structured_f.name = tmpl_name;
	//	//			NOT_IMPLEMENTED;
	//	//			/*if (uses_auto(structured_f)) {
	//	//				state.state.global_namespace.functions_using_auto[structured_f.name].push_back(std::move(structured_f));
	//	//				return expression_information{ type_information{
	//	//					.type = { NodeStructs::FunctionType{
	//	//						tmpl_name, state.state.global_namespace
	//	//					} },
	//	//					.representation = tmpl_name
	//	//				} };
	//	//			}
	//	//			else {
	//	//				state.state.global_namespace.functions[structured_f.name].push_back(copy(structured_f));
	//	//				state.state.functions_traversal.traversed.insert({ structured_f.name, copy(structured_f) });
	//	//				auto transpiled_f = transpile(state.unindented(), structured_f);
	//	//				return_if_error(transpiled_f);
	//	//				if (uses_auto(structured_f))
	//	//					NOT_IMPLEMENTED;
	//	//				state.state.functions_traversal.declarations.try_emplace({  });
	//	//				state.state.functions_to_transpile.insert(std::move(structured_f));
	//	//				return expression_information{ type_information{
	//	//					.type = { NodeStructs::FunctionType{
	//	//						tmpl_name, state.state.global_namespace
	//	//					} },
	//	//					.representation = tmpl_name
	//	//				} };
	//	//			}*/
	//	//		}
	//	//	}
	//	//	catch (...) {
	//	//		std::cout << "TEMPLATE WAS\n\n" << replaced << "\n\n";
	//	//		NOT_IMPLEMENTED;
	//	//	}
	//	//}
	//	//{
	//	//	And<IndentToken, grammar::Type, Token<END>> t{ tmpl.indent };
	//	//	auto tokens = Tokenizer(replaced).read();
	//	//	Iterator it = { .vec = tokens, .index = 0 , .line = 0, .col = 0, .file_name = "template:/" + tmpl_name };
	//	//	if (build(t, it)) {
	//	//		auto structured_t = getStruct("template:/" + tmpl_name, tokens, t.template get<grammar::Type>(), std::nullopt);
	//	//		NOT_IMPLEMENTED;
	//	//		/*auto opt_e = traverse_type(state, structured_t);
	//	//		if (opt_e.has_value())
	//	//			return opt_e.value();
	//	//		structured_t.name = tmpl_name;
	//	//		state.state.global_namespace.types[structured_t.name].push_back(copy(structured_t));
	//	//		auto opt_error = traverse_type(state, structured_t);
	//	//		if (opt_error.has_value())
	//	//			return opt_error.value();
	//	//		return expression_information{ type_information{
	//	//			.type = Realised::MetaType{ std::move(structured_t) },
	//	//			.representation = tmpl_name
	//	//		} };*/
	//	//	}
	//	//}
	//	//return error{
	//	//	"user error",
	//	//	"Template expansion does not result in a type or function: |begin|\n" + replaced + "\n|end|"
	//	//};
	//}

	if (operand == "type_list") {
		std::vector<Realised::MetaType> types;
		for (const auto& arg : expr.args) {
			expected<Realised::MetaType> arg_t = std::visit(overload(
				[&](const std::string& e) -> expected<Realised::MetaType> {
					return type_of_typename(state, variables, NodeStructs::BaseTypename{ e });
				},
				[&](const NodeStructs::Typename& t) -> expected<Realised::MetaType> {
					return type_of_typename(state, variables, t);
				},
				[&](const NodeStructs::Expression& e) -> expected<Realised::MetaType> {
					return error{
						"user error",
						"not a typename `" + original_representation(e) + "`"
					};
				}
			), arg.value._value);
			return_if_error(arg_t);
			types.push_back(std::move(arg_t).value());
		}
		return expression_information{ type_information{
			.type = Realised::MetaType{ Realised::TypeListType{.types = std::move(types) } },
			.representation = "compile time do not transpile"
		} };
	}

	if (operand == "Vector") {
		if (expr.args.size() != 1)
			NOT_IMPLEMENTED;
		expected<Realised::MetaType> arg_t = std::visit(overload(
			[&](const std::string& e) -> expected<Realised::MetaType> {
				return type_of_typename(state, variables, NodeStructs::BaseTypename{ e });
			},
			[&](const NodeStructs::Typename& t) -> expected<Realised::MetaType> {
				return type_of_typename(state, variables, t);
			},
			[&](const NodeStructs::Expression& e) -> expected<Realised::MetaType> {
				return error{
					"user error",
					"not a typename `" + original_representation(e) + "`"
				};
			}
		), expr.args[0].value._value);
		return_if_error(arg_t);

		auto tn = typename_of_type(state, arg_t.value());
		return_if_error(tn);

		auto repr = transpile_typename(state, variables, tn.value());
		return_if_error(repr);

		NOT_IMPLEMENTED;
		/*return expression_information{ type_information{
			.type = Realised::MetaType{ Realised::VectorType{.value_type = std::move(arg_t.value()) } },
			.representation = "Vector<" + repr.value() + ">"
		} };*/
	}

	return error{
		"user error",
		"not a template"
	};
}

std::optional<std::vector<non_type_information>> rearrange_if_possible(
	const auto& state,
	variables_t& variables,
	const std::vector<Realised::MetaType>& v1,
	std::vector<non_type_information>&& v2,
	size_t i1,
	size_t i2,
	std::vector<int>&& mappings
) {
	if (i1 == v1.size())
		return mappings
		| std::views::transform([&](auto&& e) -> non_type_information { return std::move(v2.at(e)); })
		| to_vec();
	if (i2 == v2.size())
		return std::nullopt;
	if (std::holds_alternative<directly_assignable>(assigned_to(state, variables, v1.at(i1), v2.at(i2).type)._value)) {
		mappings.push_back(i2);
		return rearrange_if_possible(state, variables, v1, std::move(v2), i1 + 1, 0, std::move(mappings));
	}
	else
		return rearrange_if_possible(state, variables, v1, std::move(v2), i1, i2 + 1, std::move(mappings));
}


std::optional<std::vector<non_type_information>> rearrange_if_possible(
	const auto& state,
	variables_t& variables,
	const std::vector<Realised::MetaType>& v1,
	std::vector<non_type_information>&& v2
) {
	if (v1.size() != v2.size())
		return std::nullopt;
	for (size_t i = 0; i < v1.size(); ++i)
		if (!std::holds_alternative<directly_assignable>(assigned_to(state, variables, v1.at(i), v2.at(i).type)._value))
			return rearrange_if_possible(state, variables, v1, std::move(v2), 0, 0, {});
	return std::move(v2);
}

R T::operator()(const NodeStructs::ConstructExpression& expr) {
	auto t = type_of_typename(state, variables, expr.operand);
	return_if_error(t);
	auto typename_repr = transpile_typename(state, variables, expr.operand);
	return_if_error(typename_repr);
	return std::visit(overload(
		[&](const auto& e) -> R {
			NOT_IMPLEMENTED;
		},
		[&](const Realised::PrimitiveType& e) -> R {
			if (expr.arguments.args.size() != 1)
				return error{
					"user error",
					"expected 1 argument to initialize object of type `" + transpile_typename(state, variables, typename_of_primitive(e)).value() + "`"
			};
			auto arg_info = transpile_arg(state, variables, expr.arguments.args.at(0));
			return_if_error(arg_info);
			if (!std::holds_alternative<non_type_information>(arg_info.value()))
				NOT_IMPLEMENTED;
			const auto& arg_info_ok = std::get<non_type_information>(arg_info.value());
			if (!std::holds_alternative<directly_assignable>(assigned_to(state, variables, { copy(e) }, arg_info_ok.type)._value))
				NOT_IMPLEMENTED;
			return expression_information{ non_type_information{
				.type = { copy(e) },
				.representation = transpile_typename(state, variables, typename_of_primitive(e)).value() + "{ " + arg_info_ok.representation + " }",
				.value_category = NodeStructs::Value{},
			} };
		},
		[&](const Realised::Type& tt) -> R {
			if (expr.arguments.args.size() != tt.member_variables.size())
				return error{
					"user error",
					"expected `" + std::to_string(tt.member_variables.size()) + "` arguments but received `" + std::to_string(expr.arguments.args.size()) + "` for type `" + tt.name._value + "`"
			};
			auto arg_ts = vec_of_expected_to_expected_of_vec(expr.arguments.args
				| std::views::transform([&](auto&& arg) -> R {
					return transpile_arg(state, variables, arg);
				})
				| to_vec());
			return_if_error(arg_ts);
			auto sz = arg_ts.value().size();

			auto non_type_args = std::move(arg_ts).value()
				| std::views::filter([&](const auto& e) -> bool { return std::holds_alternative<non_type_information>(e); })
				| std::views::transform([&](auto& e) -> non_type_information { return std::get<non_type_information>(std::move(e)); })
				| to_vec();
			if (non_type_args.size() != sz)
				NOT_IMPLEMENTED;

			std::vector<Realised::MetaType> param_ts = tt.member_variables
				| std::views::transform([](const Realised::MemberVariable& mem) { return copy(mem.type); })
				| to_vec();

			auto arranged = rearrange_if_possible(state, variables, param_ts, std::move(non_type_args));
			if (!arranged)
				return error{
					"user error",
					"could not arrange"
				};

			std::stringstream args_repr;
			bool has_previous = false;
			for (const auto& arg : arranged.value()) {
				if (has_previous)
					args_repr << ", ";
				else
					has_previous = true;
				args_repr << arg.representation;
			}

			return expression_information{ non_type_information{
				.type = { copy(tt) },
				.representation = typename_repr.value() + "{" + args_repr.str() + "}",
				.value_category = NodeStructs::Value{},
			} };
		},
		[&](const Realised::SetType& set_t) -> R {
			if (expr.arguments.args.size() != 0)
				NOT_IMPLEMENTED;
			return expression_information{ non_type_information{
				.type = { copy(set_t) },
				.representation = typename_repr.value() + "{}",
				.value_category = NodeStructs::Value{},
			} };
		},
		[&](const Realised::VectorType& vec_t) -> R {
			if (expr.arguments.args.size() != 0)
				return error{
					"user error",
					"vectors construction is done without arguments, expression was: `" + original_representation(expr.arguments.args[0].expr) + "`"
				};
			return expression_information{ non_type_information{
				.type = { copy(vec_t) },
				.representation = typename_repr.value() + "{}",
				.value_category = NodeStructs::Value{},
			} };
		},
		[&](const Realised::UnionType& union_t) -> R {
			if (expr.arguments.args.size() < 1)
				return error{
					"user error",
					"unions require at least one argument, union type was `" + typename_of_type(state, union_t).value().info.content + "`, expression was: `{}`"
				};
			if (expr.arguments.args.size() > 1)
				return error{
					"user error",
					"unions require at most one argument, union type was `" + typename_of_type(state, union_t).value().info.content + "`, expression was: `TODO`"
				};
			auto expr_info = transpile_arg(state, variables, expr.arguments.args.at(0));
			return_if_error(expr_info);
			if (!std::holds_alternative<non_type_information>(expr_info.value()))
				NOT_IMPLEMENTED;
			const non_type_information& expr_info_ok = std::get<non_type_information>(expr_info.value());
			for (const auto& T : union_t.arguments)
				if (cmp(T, expr_info_ok.type) == std::strong_ordering::equivalent)
					return expression_information{ non_type_information{
						.type = { copy(union_t) },
						.representation = typename_repr.value() + "{" + expr_info_ok.representation + "}",
						.value_category = NodeStructs::Value{},
					} };
			return error{
				"user error",
				"expression does not match any of the union type"
			};
		}
	), t.value().type.get()._value);
}

R T::operator()(const NodeStructs::BracketAccessExpression& expr) {
	auto operand_info = transpile_expression(state, variables, expr.operand);
	return_if_error(operand_info);
	if (!std::holds_alternative<non_type_information>(operand_info.value()))
		NOT_IMPLEMENTED;
	auto [type, repr, vcat] = std::get<non_type_information>(std::move(operand_info).value());
	NOT_IMPLEMENTED;
	/*return std::visit(overload(
		[&](const auto& x) -> R {
			return error{
				"user error",
				"bracket access is reserved for vector, set, map and tuple types"
			};
		},
		[&](NodeStructs::VectorType&& vt) -> R {
			auto arg_info = transpile_arg(state, variables, expr.arguments.args.at(0));
			return_if_error(arg_info);
			if (!std::holds_alternative<non_type_information>(arg_info.value()))
				NOT_IMPLEMENTED;
			const non_type_information& arg_info_ok = std::get<non_type_information>(arg_info.value());
			if (cmp(arg_info_ok.type, Realised::MetaType{ Realised::PrimitiveType{ Realised::PrimitiveType::NonValued<int>{} } }) != std::strong_ordering::equivalent)
				NOT_IMPLEMENTED;
			return expression_information{ non_type_information{
				.type = std::move(vt).value_type,
				.representation = std::move(repr) + "[" + arg_info_ok.representation + "]",
				.value_category = std::move(vcat),
			} };
		},
		[&](NodeStructs::MapType&& vt) -> R {
			auto arg_info = transpile_arg(state, variables, expr.arguments.args.at(0));
			return_if_error(arg_info);
			if (!std::holds_alternative<non_type_information>(arg_info.value()))
				NOT_IMPLEMENTED;
			const non_type_information& arg_info_ok = std::get<non_type_information>(arg_info.value());
			if (cmp(arg_info_ok.type, vt.key_type) != std::strong_ordering::equivalent)
				NOT_IMPLEMENTED;
			return expression_information{ non_type_information{
				.type = std::move(vt).value_type,
				.representation = std::move(repr) + "[" + arg_info_ok.representation + "]",
				.value_category = std::move(vcat),
			} };
		}
	), std::move(type).type.get()._value);*/
}

// for expressions like (move cat).meow() or {move cat}.meow()
auto rewire(const NodeStructs::PropertyAccessAndCallExpression& expr, const auto& operand) {
	std::vector<NodeStructs::FunctionArgument> joined;
	for (const auto& arg : operand.args)
		joined.push_back(copy(arg));
	for (const auto& arg : expr.arguments.args)
		joined.push_back(copy(arg));
	return NodeStructs::CallExpression{
		.operand = make_expression({ expr.property_name }, rule_info_stub_no_throw()),
		.arguments = { std::move(joined) }
	};
}

R T::operator()(const NodeStructs::PropertyAccessAndCallExpression& expr) {
	if (holds<NodeStructs::ParenArguments>(expr.operand)) {
		const auto& operand = get<NodeStructs::ParenArguments>(expr.operand);
		return operator()(rewire(expr, operand));
	}
	if (holds<NodeStructs::BraceArguments>(expr.operand)) {
		const auto& operand = get<NodeStructs::BraceArguments>(expr.operand);
		return operator()(rewire(expr, operand));
	}
	auto operand_info = transpile_expression(state, variables, expr.operand);
	return_if_error(operand_info);
	if (!std::holds_alternative<non_type_information>(operand_info.value()))
		NOT_IMPLEMENTED;
	const non_type_information& operand_info_ok = std::get<non_type_information>(operand_info.value());
	return transpile_member_call(state, variables, operand_info_ok, expr.property_name, expr.arguments.args);
}

R T::operator()(const NodeStructs::PropertyAccessExpression& expr) {
	auto operand_info = transpile_expression(state, variables, expr.operand);
	return_if_error(operand_info);
	if (!std::holds_alternative<non_type_information>(operand_info.value()))
		NOT_IMPLEMENTED;
	auto [type, repr, vcat] = std::get<non_type_information>(std::move(operand_info).value());

	auto t = type_of_postfix_member(state, variables, expr.property_name, type);
	return_if_error(t);

	if (holds<Realised::InterfaceType>(type.type.get()._value))
		return expression_information{ non_type_information{
			.type = std::move(t).value().second,
			.representation = "std::visit(overload([&](auto&& XX){ return XX." + expr.property_name + "; }), " + std::move(repr) + ".value)",
			.value_category = std::move(vcat),
		} };
	else
		return expression_information{ non_type_information{
			.type = std::move(t).value().second,
			.representation = std::move(repr) + "." + expr.property_name,
			.value_category = std::move(vcat),
		} };
}

R T::operator()(const NodeStructs::ParenArguments& expr) {
	if (expr.args.size() != 1)
		NOT_IMPLEMENTED;
	auto inner = transpile_arg(state, variables, expr.args.at(0));
	return_if_error(inner);
	if (!std::holds_alternative<non_type_information>(inner.value()))
		NOT_IMPLEMENTED;
	non_type_information inner_ok = std::get<non_type_information>(std::move(inner).value());
	return expression_information{ non_type_information{
		.type = std::move(inner_ok).type,
		.representation = "(" + inner_ok.representation + ")",
		.value_category = std::move(inner_ok).value_category
	} };
}

R T::operator()(const NodeStructs::BraceArguments& expr) {
	auto vec = expr.args
		| std::views::transform([&](auto&& arg) { return transpile_arg(state, variables, arg); })
		| to_vec();
	auto args_ = vec_of_expected_to_expected_of_vec(std::move(vec));
	return_if_error(args_);
	auto args_ok_maybe_wrong_type = std::move(args_).value();
	auto opt = vec_of_variant_to_optional_vector_single_type<non_type_information>(std::move(args_ok_maybe_wrong_type));
	if (!opt.has_value())
		NOT_IMPLEMENTED;
	auto args_ok = std::move(opt).value();

	std::stringstream ss;
	ss << "{ ";
	for (size_t i = 0; i < expr.args.size(); ++i)
		ss << args_ok.at(i).representation << ", ";
	ss << "}";
	NOT_IMPLEMENTED;
	//return expression_information{ non_type_information{
	//	.type = { Realised::AggregateType{
	//		copy(expr.args),
	//		args_ok | std::views::transform([](non_type_information& e) { return std::move(e).type; }) | to_vec()
	//	} },
	//	.representation = ss.str(),
	//	.value_category = NodeStructs::Value{}, // todo check conversion ok
	//} };
}

R T::operator()(const std::string& expr) {
	if (auto it = variables.find(expr); it != variables.end() && it->second.size() > 0) {
		if (it->second.size() != 1)
			NOT_IMPLEMENTED;
		const auto& v = it->second.back();
		return expression_information{ non_type_information{
			.type = copy(v.type),
			.representation = expr,
			.value_category = copy(v.value_category),
		} };
	}

	bool has_f = find_by_name(state.state.global_namespace.functions, expr) != state.state.global_namespace.functions.end();
	if (has_f)
		return expression_information{ type_information{
			.type = Realised::MetaType{ Realised::FunctionType{
				expr
			} },
			.representation = expr
		} };
	if (auto it = find_by_name(state.state.global_namespace.types, expr); it != state.state.global_namespace.types.end()) {
		const auto& T = *it;
		auto rt_or_e = realise_type(state, T);
		return_if_error(rt_or_e);
		return expression_information{ type_information{
			.type = Realised::MetaType{ std::move(rt_or_e.value()) },
			.representation = expr
		} };
	}
	if (auto it = find_by_name(state.state.global_namespace.aliases, expr); it != state.state.global_namespace.aliases.end()) {
		const auto& type_name = it->aliasTo;
		if (std::optional<error> err = realise_typename(state, type_name); err.has_value())
			return err.value();
		auto type = type_of_typename(state, variables, type_name);
		return_if_error(type);
		return expression_information{ type_information{
			.type = std::move(type).value(),
			.representation = transpile_typename(state, variables, type_name).value()
		} };
	}
	if (auto it = find_by_name(state.state.global_namespace.templates, expr); it != state.state.global_namespace.templates.end())
		NOT_IMPLEMENTED;
		/*return expression_information{ type_information{
			.type = { Realised::TemplateType{ expr, state.state.global_namespace } },
			.representation = expr
		} };*/
	if (auto it = find_by_name(state.state.global_namespace.namespaces, expr); it != state.state.global_namespace.namespaces.end())
		NOT_IMPLEMENTED;
		/*return expression_information{ type_information{
			.type = { Realised::NamespaceType{ it->second } },
			.representation = expr
		} };*/
	if (auto it = find_by_name(state.state.global_namespace.enums, expr); it != state.state.global_namespace.enums.end())
		NOT_IMPLEMENTED;
		/*return expression_information{ type_information{
			.type = { Realised::EnumType{ it->second.back() } },
			.representation = expr
		} };*/
	return error{ "user error", "Undeclared identifier `" + expr + "`" };
}

R T::operator()(const Token<INTEGER_NUMBER>& expr) {
	return expression_information{ non_type_information{
		.type = { Realised::PrimitiveType{ Realised::PrimitiveType::Valued<int>{ atoi(expr.value.c_str()) } } },
		.representation = expr.value,
		.value_category = NodeStructs::Value{}
	} };
}

R T::operator()(const Token<FLOATING_POINT_NUMBER>& expr) {
	return expression_information{ non_type_information{
		.type = { Realised::PrimitiveType{ Realised::PrimitiveType::Valued<double>{ stod(expr.value) } } },
		.representation = expr.value,
		.value_category = NodeStructs::Value{}
	} };
}

R T::operator()(const Token<STRING>& expr) {
	if (expr.value.size() == 3) // char
		return expression_information{ non_type_information{
			.type = { Realised::PrimitiveType{ Realised::PrimitiveType::Valued<char>{ expr.value.at(1) } } },
			.representation = "Char{ '" + std::string{ expr.value.at(1) } + "' }",
			.value_category = NodeStructs::Value{}
		} };
	if (expr.value.size() == 4 && expr.value.at(1) == '\\') { // backslash character
		if (expr.value.at(2) == '"')
			return expression_information{ non_type_information{
				.type = { Realised::PrimitiveType{ Realised::PrimitiveType::Valued<char>{ expr.value.at(0) } } },
				.representation = "Char{ '" + std::string{ expr.value.at(2) } + "' }",
				.value_category = NodeStructs::Value{}
			} };
		else
			return expression_information{ non_type_information{
				.type = { Realised::PrimitiveType{ Realised::PrimitiveType::Valued<char>{ expr.value.at(0) } } },
				.representation = "Char{ '\\" + std::string{ expr.value.at(2) } + "' }",
				.value_category = NodeStructs::Value{}
			} };
	}
	else
		return expression_information{ non_type_information{
			.type = { Realised::PrimitiveType{ Realised::PrimitiveType::Valued<std::string>{ expr.value } } },
			.representation = "String{" + expr.value + "}",
			.value_category = NodeStructs::Value{}
		} };
}
