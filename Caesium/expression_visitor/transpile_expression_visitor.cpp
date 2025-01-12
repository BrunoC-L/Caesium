#include "../core/toCPP.hpp"
#include "../utility/replace_all.hpp"
#include "../core/structurizer.hpp"
#include "../utility/vec_of_expected_to_expected_of_vec.hpp"
#include "../utility/vec_of_variant_to_optional_vector_single_type.hpp"
#include <iostream>
#include <algorithm>

using T = transpile_expression_visitor;
using R = T::R;

bool is_int(const NodeStructs::MetaType& t) {
	return holds<NodeStructs::PrimitiveType>(t)
		&& (holds<NodeStructs::PrimitiveType::Valued<int>>(get<NodeStructs::PrimitiveType>(t).value)
			|| holds<NodeStructs::PrimitiveType::NonValued<int>>(get<NodeStructs::PrimitiveType>(t).value));
}

bool is_bool(const NodeStructs::MetaType& t) {
	return holds<NodeStructs::PrimitiveType>(t)
		&& (holds<NodeStructs::PrimitiveType::Valued<bool>>(get<NodeStructs::PrimitiveType>(t).value)
			|| holds<NodeStructs::PrimitiveType::NonValued<bool>>(get<NodeStructs::PrimitiveType>(t).value));
}

bool is_floating(const NodeStructs::MetaType& t) {
	return holds<NodeStructs::PrimitiveType>(t)
		&& (holds<NodeStructs::PrimitiveType::Valued<double>>(get<NodeStructs::PrimitiveType>(t).value)
			|| holds<NodeStructs::PrimitiveType::NonValued<double>>(get<NodeStructs::PrimitiveType>(t).value));
}

bool is_char_or_string(const NodeStructs::MetaType& t) {
	return holds<NodeStructs::PrimitiveType>(t)
		&& (holds<NodeStructs::PrimitiveType::Valued<std::string>>(get<NodeStructs::PrimitiveType>(t).value)
			|| holds<NodeStructs::PrimitiveType::NonValued<std::string>>(get<NodeStructs::PrimitiveType>(t).value)
			|| holds<NodeStructs::PrimitiveType::Valued<char>>(get<NodeStructs::PrimitiveType>(t).value)
			|| holds<NodeStructs::PrimitiveType::NonValued<char>>(get<NodeStructs::PrimitiveType>(t).value));
}

R T::operator()(const NodeStructs::ConditionalExpression& expr) {
	if (expr.ifElseExprs.has_value()) {
		// x if first else second
		//                  ([&] () { if (first) return x; else return second; }());

		auto condition_expr_info = operator()(expr.ifElseExprs.value().first);
		return_if_error(condition_expr_info);
		if (!std::holds_alternative<non_type_information>(condition_expr_info.value()))
			throw;
		const non_type_information& condition_expr_info_ok = std::get<non_type_information>(condition_expr_info.value());

		auto if_expr_info = operator()(expr.expr);
		return_if_error(if_expr_info);
		if (!std::holds_alternative<non_type_information>(if_expr_info.value()))
			throw;
		non_type_information& if_expr_info_ok = std::get<non_type_information>(condition_expr_info.value());
		if (cmp(if_expr_info_ok.type, NodeStructs::MetaType{ NodeStructs::PrimitiveType{ NodeStructs::PrimitiveType::NonValued<bool>{} } }) != std::weak_ordering::equivalent)
		throw;

		auto else_expr_info = operator()(expr.ifElseExprs.value().second);
		return_if_error(else_expr_info);
		if (!std::holds_alternative<non_type_information>(else_expr_info.value()))
			throw;
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
		throw;
	const non_type_information& base_ok = std::get<non_type_information>(base.value());
	if (cmp(base_ok.type, NodeStructs::MetaType{ NodeStructs::PrimitiveType{ NodeStructs::PrimitiveType::NonValued<bool>{} } }) != std::weak_ordering::equivalent)
	throw;
	ss << base_ok.representation;
	for (const auto& e : expr.ors) {
		auto or_expr = operator()(e);
		return_if_error(or_expr);
		if (!std::holds_alternative<non_type_information>(or_expr.value()))
			throw;
		const non_type_information& or_expr_ok = std::get<non_type_information>(or_expr.value());
		if (cmp(or_expr_ok.type, NodeStructs::MetaType{ NodeStructs::PrimitiveType{ NodeStructs::PrimitiveType::NonValued<bool>{} } }) != std::weak_ordering::equivalent)
		throw;
		ss << " || " << or_expr_ok.representation;
	}
	return expression_information{ non_type_information{
		.type = NodeStructs::PrimitiveType{ NodeStructs::PrimitiveType::NonValued<bool>{} },
		.representation = ss.str(),
		.value_category = NodeStructs::Value{},
	} };
}

R T::operator()(const NodeStructs::AndExpression& expr) {
	std::stringstream ss;
	auto base = operator()(expr.expr);
	return_if_error(base);
	if (!std::holds_alternative<non_type_information>(base.value()))
		throw;
	const non_type_information& base_ok = std::get<non_type_information>(base.value());
	if (cmp(base_ok.type, NodeStructs::MetaType{ NodeStructs::PrimitiveType{ NodeStructs::PrimitiveType::NonValued<bool>{} } }) != std::weak_ordering::equivalent)
		throw;
	ss << base_ok.representation;
	for (const auto& e : expr.ands) {
		auto and_ = operator()(e);
		return_if_error(and_);
		if (!std::holds_alternative<non_type_information>(and_.value()))
			throw;
		const non_type_information& and_ok = std::get<non_type_information>(and_.value());
		if (cmp(and_ok.type, NodeStructs::MetaType{ NodeStructs::PrimitiveType{ NodeStructs::PrimitiveType::NonValued<bool>{} } }) != std::weak_ordering::equivalent)
			throw;
		ss << " || " << and_ok.representation;
	}
	return expression_information{ non_type_information{
		.type = NodeStructs::PrimitiveType{ NodeStructs::PrimitiveType::NonValued<bool>{} },
		.representation = ss.str(),
		.value_category = NodeStructs::Value{},
	} };
}

R T::operator()(const NodeStructs::EqualityExpression& expr) {
	if (expr.equals.size() != 1)
		throw;
	auto base = operator()(expr.expr);
	return_if_error(base);
	if (std::holds_alternative<non_type_information>(base.value())) {
		std::stringstream ss;
		const non_type_information& base_ok = std::get<non_type_information>(base.value());
		if (!holds<NodeStructs::PrimitiveType>(base_ok.type) && !holds<NodeStructs::EnumType>(base_ok.type))
			throw;
		ss << base_ok.representation;
		for (const auto& [op, e] : expr.equals) {
			auto eq = operator()(e);
			return_if_error(eq);
			if (!std::holds_alternative<non_type_information>(eq.value()))
				throw;
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
			.type = NodeStructs::PrimitiveType{ NodeStructs::PrimitiveType::NonValued<bool>{} },
			.representation = ss.str(),
			.value_category = NodeStructs::Value{},
		} };
	}
	else {
		const type_information& left = std::get<type_information>(base.value());
		if (expr.equals.size() != 1)
			throw;
		const auto& [op, e] = expr.equals[0];
		auto right = operator()(e);
		return_if_error(right);
		if (!std::holds_alternative<type_information>(right.value()))
			throw;
		const type_information& right_ok = std::get<type_information>(right.value());
		auto [str, val] = [&]() -> std::pair<std::string, bool> {
			if (cmp(left.type, right_ok.type) == std::weak_ordering::equivalent)
				return { "True", true };
			else
				return { "False", false };
		}();
		return expression_information{ non_type_information{
			.type = NodeStructs::PrimitiveType{ NodeStructs::PrimitiveType::Valued<bool>{ val } },
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
		throw;
	const non_type_information& base_ok = std::get<non_type_information>(base.value());
	if (!is_int(base_ok.type) && !is_floating(base_ok.type) && !is_char_or_string(base_ok.type))
		throw;
	ss << base_ok.representation;
	if (expr.comparisons.size() != 1)
		throw; // todo implement a > b > c, which is not (a>b)>c but rather (a>b)&&(b>c)
	for (const auto& [op, e] : expr.comparisons) {
		auto cmp_ = operator()(e);
		return_if_error(cmp_);
		if (!std::holds_alternative<non_type_information>(cmp_.value()))
			throw;
		const non_type_information& cmp_ok = std::get<non_type_information>(cmp_.value());
		if (!holds<NodeStructs::PrimitiveType>(cmp_ok.type))
			throw;
		if (get<NodeStructs::PrimitiveType>(base_ok.type).value._value.index() == get<NodeStructs::PrimitiveType>(cmp_ok.type).value._value.index()
			|| get<NodeStructs::PrimitiveType>(base_ok.type).value._value.index() + 7 == get<NodeStructs::PrimitiveType>(cmp_ok.type).value._value.index()
			|| get<NodeStructs::PrimitiveType>(base_ok.type).value._value.index() - 7 == get<NodeStructs::PrimitiveType>(cmp_ok.type).value._value.index())
				ss << " " << symbol_variant_as_text(op._value) << " " << cmp_ok.representation;
		else
			return error{ "user error", "cannot compare types for CompareExpression" };
	}
	return expression_information{ non_type_information{
		.type = NodeStructs::PrimitiveType{ NodeStructs::PrimitiveType::NonValued<bool>{} },
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
			throw;
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
		.type = NodeStructs::MetaType{
			has_floating ?
			NodeStructs::MetaType{ NodeStructs::PrimitiveType{ NodeStructs::PrimitiveType::NonValued<double>{} } }
			: NodeStructs::MetaType{ NodeStructs::PrimitiveType{ NodeStructs::PrimitiveType::NonValued<int>{} } }
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
			throw;
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
		.type = NodeStructs::MetaType{ NodeStructs::PrimitiveType{ NodeStructs::PrimitiveType::NonValued<std::string>{} } },
		.representation = ss.str(),
		.value_category = NodeStructs::Value{},
	} };
}

R T::operator()(const NodeStructs::AdditiveExpression& expr) {
	std::stringstream ss;
	auto base = operator()(expr.expr);
	return_if_error(base);
	if (!std::holds_alternative<non_type_information>(base.value()))
		throw;
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
		throw;
	const non_type_information& base_ok = std::get<non_type_information>(base.value());
	if (!is_int(base_ok.type) && !is_floating(base_ok.type))
		return error{ "user error", "multiplication is only available for Int and Floating" };
	ss << base_ok.representation;

	for (const auto& [op, e] : expr.muls) {
		auto mul = operator()(e);
		return_if_error(mul);
		if (!std::holds_alternative<non_type_information>(mul.value()))
			throw;
		const non_type_information& mul_ok = std::get<non_type_information>(mul.value());
		if (!is_int(mul_ok.type) && !is_floating(mul_ok.type))
			return error{ "user error", "multiplication is only available for Int and Floating" };
		ss << " " << symbol_variant_as_text(op._value) << " " << mul_ok.representation;
	}

	return expression_information{ non_type_information{
		.type = NodeStructs::MetaType{ NodeStructs::PrimitiveType{ NodeStructs::PrimitiveType::NonValued<int>{} } },
		.representation = ss.str(),
		.value_category = NodeStructs::Value{},
	} };
}

R T::operator()(const NodeStructs::UnaryExpression& expr) {
	if (expr.unary_operators.size() != 1)
		throw;

	/*for (const NodeStructs::UnaryExpression::op_types& op : expr.unary_operators)
		ss << symbol_variant_as_text(op._value);*/

	auto base = operator()(expr.expr);
	return_if_error(base);
	if (!std::holds_alternative<non_type_information>(base.value()))
		throw;
	const non_type_information& base_ok = std::get<non_type_information>(base.value());

	if (holds<NodeStructs::CompileTimeType>(base_ok.type)) {
		if (holds<Token<DASH>>(expr.unary_operators[0])) {
			if (!holds<NodeStructs::PrimitiveType>(get<NodeStructs::CompileTimeType>(base_ok.type).type))
				throw;
			if (holds<NodeStructs::PrimitiveType::Valued<int>>(get<NodeStructs::PrimitiveType>(get<NodeStructs::CompileTimeType>(base_ok.type).type).value)) {
				int val = get<NodeStructs::PrimitiveType::Valued<int>>(get<NodeStructs::PrimitiveType>(get<NodeStructs::CompileTimeType>(base_ok.type).type).value).value;
				return expression_information{ non_type_information{
					.type = NodeStructs::PrimitiveType{ NodeStructs::PrimitiveType::Valued<int>{ -val } },
					.representation = std::to_string(-val),
					.value_category = NodeStructs::Value{},
				} };
			}
			if (holds<NodeStructs::PrimitiveType::Valued<double>>(get<NodeStructs::PrimitiveType>(get<NodeStructs::CompileTimeType>(base_ok.type).type).value)) {
				double val = get<NodeStructs::PrimitiveType::Valued<double>>(get<NodeStructs::PrimitiveType>(get<NodeStructs::CompileTimeType>(base_ok.type).type).value).value;
				return expression_information{ non_type_information{
					.type = NodeStructs::PrimitiveType{ NodeStructs::PrimitiveType::Valued<double>{ -val } },
					.representation = std::to_string(-val),
					.value_category = NodeStructs::Value{},
				} };
			}
			throw;
		}
		if (holds<Token<NOT>>(expr.unary_operators[0])) {
			if (!holds<NodeStructs::PrimitiveType>(get<NodeStructs::CompileTimeType>(base_ok.type).type))
				throw;
			if (holds<NodeStructs::PrimitiveType::Valued<bool>>(get<NodeStructs::PrimitiveType>(get<NodeStructs::CompileTimeType>(base_ok.type).type).value)) {
				bool val = get<NodeStructs::PrimitiveType::Valued<bool>>(get<NodeStructs::PrimitiveType>(get<NodeStructs::CompileTimeType>(base_ok.type).type).value).value;
				return expression_information{ non_type_information{
					.type = NodeStructs::PrimitiveType{ NodeStructs::PrimitiveType::Valued<bool>{ !val } },
					.representation = std::to_string(!val),
					.value_category = NodeStructs::Value{},
				} };
			}
			throw;
		}
		throw;
	}
	else {
		if (holds<Token<DASH>>(expr.unary_operators[0]) && !(is_int(base_ok.type) || is_floating(base_ok.type)))
			throw;
		if (holds<Token<NOT>>(expr.unary_operators[0]) && !is_bool(base_ok.type))
			throw;
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
		const std::string& operand = get<std::string>(expr.operand);
		if (auto it = state.state.global_namespace.functions_using_auto.find(operand); it != state.state.global_namespace.functions_using_auto.end()) {
			auto args_ = vec_of_expected_to_expected_of_vec(expr.arguments.args
				| std::views::transform([&](auto&& arg) { return transpile_arg(state, variables, arg); })
				| to_vec());
			return_if_error(args_);
			auto args_ok_maybe_wrong_type = std::move(args_).value();
			auto opt = vec_of_variant_to_optional_vector_single_type<non_type_information>(std::move(args_ok_maybe_wrong_type));
			if (!opt.has_value())
				throw;
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
				if (cmp(f, fn.value()) == std::weak_ordering::equivalent)
					found = true;
			if (!found)
				vec.push_back(std::move(fn).value());
		}
	}
	auto t = operator()(expr.operand);
	return_if_error(t);
	if (std::holds_alternative<type_information>(t.value())) {
		const auto& ti = std::get<type_information>(t.value());
		if (holds<NodeStructs::Builtin>(ti.type)) {
			const std::string& name = get<NodeStructs::Builtin>(ti.type).name;
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
					.type = NodeStructs::MetaType{ NodeStructs::PrimitiveType{ NodeStructs::PrimitiveType::NonValued<NodeStructs::void_t>{} } },
					.representation = ss.str(),
					.value_category = NodeStructs::Value{}
				} };
			}
			if (name == "exit") {
				std::stringstream ss;
				ss << "exit(";
				if (expr.arguments.args.size() != 1)
					throw;
				auto expr_info = operator()(expr.arguments.args.at(0).expr);
				return_if_error(expr_info);
				if (!std::holds_alternative<non_type_information>(expr_info.value()))
					throw;
				const non_type_information& expr_info_ok = std::get<non_type_information>(expr_info.value());
				if (!std::holds_alternative<directly_assignable>(assigned_to(state, variables, NodeStructs::MetaType{ NodeStructs::PrimitiveType{ NodeStructs::PrimitiveType::NonValued<int>{} } }, expr_info_ok.type)._value))
					throw;
				ss << expr_info_ok.representation << ")";
				return expression_information{ non_type_information{
					.type = NodeStructs::MetaType{ NodeStructs::PrimitiveType{ NodeStructs::PrimitiveType::NonValued<NodeStructs::void_t>{} } },
					.representation = ss.str(),
					.value_category = NodeStructs::Value{}
				} };
			}
			if (name == "compile_time_error") {
				return error{
					"user error",
					"user activated compile_time_error encountered"
				};
			}
		}
		if (holds<NodeStructs::TemplateType>(ti.type)) {
			const auto& options = get<NodeStructs::TemplateType>(ti.type)
				.name_space.get()
				.templates.at(get<NodeStructs::TemplateType>(ti.type).name);
			if (options.size() != 1)
				throw;
			const auto& tmpl = options.back();
			/*auto arg_ts = vec_of_expected_to_expected_of_vec(
				expr.arguments.args
				| std::views::transform([&](auto&& e) { return e.expr; })
				| std::views::transform([&](auto&& e) { return transpile_expression(state, e); })
				| to_vec()
			);*/
			throw;
		}
		if (holds<NodeStructs::FunctionType>(ti.type)) {
			auto args_ = vec_of_expected_to_expected_of_vec(expr.arguments.args
				| std::views::transform([&](auto&& arg) { return transpile_arg(state, variables, arg); })
				| to_vec());
			return_if_error(args_);
			auto args_ok_maybe_wrong_type = std::move(args_).value();
			auto opt = vec_of_variant_to_optional_vector_single_type<non_type_information>(std::move(args_ok_maybe_wrong_type));
			if (!opt.has_value())
				throw;
			const auto& args_ok = opt.value();
			auto expected_f = find_best_function(
				state,
				variables,
				get<NodeStructs::FunctionType>(ti.type).name,
				get<NodeStructs::FunctionType>(ti.type).name_space,
				args_ok | std::views::transform([](const auto& e) { return copy(e.type); }) | to_vec()
			);
			return_if_error(expected_f);
			if (!expected_f.value().has_value())
				return error{ "user error", "no matching function" };
			const NodeStructs::Function& fn = *expected_f.value().value();
			if (!state.state.traversed_functions.contains(fn)) {
				state.state.traversed_functions.insert(copy(fn));
				auto transpiled_f = transpile(state.unindented(), fn);
				return_if_error(transpiled_f);
				if (uses_auto(fn))
					throw;
				state.state.functions_to_transpile.insert(copy(fn));
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
			} };
		}
		throw;
	}
	throw;
}

R T::operator()(const NodeStructs::NamespaceExpression& expr) {
	transpile_expression_information_t operand_t = operator()(expr.name_space);
	return_if_error(operand_t);
	if (!std::holds_alternative<type_information>(operand_t.value()))
		throw;
	const auto& operand_t_ok = std::get<type_information>(operand_t.value());
	return std::visit(overload(
		[&](const NodeStructs::NamespaceType& nst) -> transpile_expression_information_t {
			const auto& ns = nst.name_space.get();
			if (auto it = ns.types.find(expr.name_in_name_space); it != ns.types.end())
				return expression_information{ type_information{
					.type = copy(it->second.back()),
					.representation = operand_t_ok.representation + "__" + expr.name_in_name_space
				} };
			if (auto it = ns.functions.find(expr.name_in_name_space); it != ns.functions.end())
				return expression_information{ type_information{
					.type = NodeStructs::MetaType{ NodeStructs::FunctionType{
						.name = expr.name_in_name_space,
						.name_space = ns
					} },
					.representation = operand_t_ok.representation + "__" + expr.name_in_name_space
				} };
			if (auto it = ns.templates.find(expr.name_in_name_space); it != ns.templates.end())
				return expression_information{ type_information{
					.type = NodeStructs::MetaType{ NodeStructs::TemplateType{
						.name = expr.name_in_name_space,
						.name_space = ns
					} },
					.representation = operand_t_ok.representation + "__" + expr.name_in_name_space
				} };
			return error{
				"user error",
				"namespace `" + ns.name + "` has no type or function `" + expr.name_in_name_space + "`"
			};
		},
		[&](const NodeStructs::EnumType& enumt) -> transpile_expression_information_t {
			const auto& enum_ = enumt.enum_.get();
			if (auto it = std::find(enum_.values.begin(), enum_.values.end(), expr.name_in_name_space); it != enum_.values.end()) {
				return expression_information{ non_type_information{
					.type = NodeStructs::EnumValueType{ enum_, expr.name_in_name_space },
					.representation = operand_t_ok.representation + "__" + expr.name_in_name_space,
					.value_category = NodeStructs::Value{}
				} };
			}
			return error{
				"user error",
				"enum `" + operand_t_ok.representation + "` has no member `" + expr.name_in_name_space + "`"
			};
		},
		[&](const auto& nst) -> transpile_expression_information_t {
			throw;
		}
	), operand_t_ok.type.type.get()._value);
}

R T::operator()(const NodeStructs::TemplateExpression& expr) {
	if (!holds<std::string>(expr.operand))
		throw;
	const auto& operand = get<std::string>(expr.operand);

	if (auto it = state.state.global_namespace.templates.find(operand);
		it != state.state.global_namespace.templates.end()) {
		auto t = find_best_template(state, variables, it->second, expr.args);
		return_if_error(t);
		const auto& tmpl = t.value().tmpl.get();
		expected<std::vector<std::string>> args = vec_of_expected_to_expected_of_vec(expr.args
			| std::views::transform([&](auto&& e) { return word_typename_or_expression_for_template(state, variables, e); })
			| to_vec());
		return_if_error(args);
		auto tmpl_name_or_e = word_typename_or_expression_for_template(
			state,
			variables,
			{ NodeStructs::Expression{ .expression = copy(expr), .rule_info = copy(t.value().tmpl.get().rule_info) }}
		);
		return_if_error(tmpl_name_or_e);
		const auto& tmpl_name = tmpl_name_or_e.value();

		size_t max_params = it->second.at(0).parameters.size();
		for (unsigned i = 1; i < it->second.size(); ++i)
			max_params = std::max(max_params, it->second.at(i).parameters.size());

		const auto& arg_placements = t.value().arg_placements;
		auto grab_nth_with_commas = [&](size_t i) {
			std::stringstream ss;
			bool has_previous = false;
			for (size_t j = 0; j < arg_placements.size(); ++j) {
				size_t k = arg_placements.at(j);
				if (k == i) {
					const auto& arg = args.value().at(j);
					if (has_previous)
						ss << ", ";
					has_previous = true;
					ss << arg;
				}
			}
			return ss.str();
		};
		std::string replaced = tmpl.templated;
		for (size_t i = 0; i < tmpl.parameters.size(); ++i) {
			replaced = replace_all(
				std::move(replaced),
				std::visit(overload(
					[](const auto& e) { return "`" + e.name + "`"; },
					[](const NodeStructs::VariadicTemplateParameter& e) { return "`" + e.name + "...`"; }
				), tmpl.parameters.at(i)._value),
				grab_nth_with_commas(i)
			);
		}

		bool has_f = state.state.global_namespace.functions.find(tmpl_name) != state.state.global_namespace.functions.end();
		bool has_f_using_auto = state.state.global_namespace.functions_using_auto.find(tmpl_name) != state.state.global_namespace.functions_using_auto.end();
		if (has_f || has_f_using_auto)
			return expression_information{ type_information{
				.type = NodeStructs::MetaType{ NodeStructs::FunctionType{
					tmpl_name, state.state.global_namespace
				} },
				.representation = tmpl_name
			} };

		if (auto it = state.state.global_namespace.types.find(tmpl_name); it != state.state.global_namespace.types.end()) {
			return expression_information{ type_information{
				.type = NodeStructs::MetaType{ copy(it->second.back()) },
				.representation = tmpl_name
			} };
		}

		{
			And<IndentToken, grammar::Function, Token<END>> f{ tmpl.indent };
			auto tokens = Tokenizer(replaced).read();
			Iterator it = { .vec = tokens, .index = 0 , .line = 0, .col = 0, .file_name = "template:/" + tmpl_name };
			try {
				if (build(f, it)) {
					auto structured_f = getStruct("template:/" + tmpl_name, tokens, f.get<grammar::Function>(), std::nullopt);
					structured_f.name = tmpl_name;
					if (uses_auto(structured_f)) {
						state.state.global_namespace.functions_using_auto[structured_f.name].push_back(std::move(structured_f));
						return expression_information{ type_information{
							.type = NodeStructs::FunctionType{
								tmpl_name, state.state.global_namespace
							},
							.representation = tmpl_name
						} };
					}
					else {
						state.state.global_namespace.functions[structured_f.name].push_back(copy(structured_f));
						state.state.traversed_functions.insert(copy(structured_f));
						auto transpiled_f = transpile(state.unindented(), structured_f);
						return_if_error(transpiled_f);
						if (uses_auto(structured_f))
							throw;
						state.state.functions_to_transpile.insert(std::move(structured_f));
						return expression_information{ type_information{
							.type = NodeStructs::FunctionType{
								tmpl_name, state.state.global_namespace
							},
							.representation = tmpl_name
						} };
					}
				}
			}
			catch (...) {
				std::cout << "TEMPLATE WAS\n\n" << replaced << "\n\n";
				throw;
			}
		}
		{
			And<IndentToken, grammar::Type, Token<END>> t{ tmpl.indent };
			auto tokens = Tokenizer(replaced).read();
			Iterator it = { .vec = tokens, .index = 0 , .line = 0, .col = 0, .file_name = "template:/" + tmpl_name };
			if (build(t, it)) {
				auto structured_t = getStruct("template:/" + tmpl_name, tokens, t.get<grammar::Type>(), std::nullopt);
				structured_t.name = tmpl_name;
				state.state.global_namespace.types[structured_t.name].push_back(copy(structured_t));
				auto opt_error = traverse_type(state, structured_t);
				if (opt_error.has_value())
					return opt_error.value();
				return expression_information{ type_information{
					.type = NodeStructs::MetaType{ std::move(structured_t) },
					.representation = tmpl_name
				} };
			}
		}
		return error{
			"user error",
			"Template expansion does not result in a type or function: |begin|\n" + replaced + "\n|end|"
		};
	}

	if (operand == "type_list") {
		std::vector<NodeStructs::MetaType> types;
		for (const auto& arg : expr.args) {
			expected<NodeStructs::MetaType> arg_t = std::visit(overload(
				[&](const std::string& e) -> expected<NodeStructs::MetaType> {
					return type_of_typename(state, variables, NodeStructs::BaseTypename{ e });
				},
				[&](const NodeStructs::Typename& t) -> expected<NodeStructs::MetaType> {
					return type_of_typename(state, variables, t);
				},
				[&](const NodeStructs::Expression& e) -> expected<NodeStructs::MetaType> {
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
			.type = NodeStructs::MetaType{ NodeStructs::TypeListType{.types = std::move(types) } },
			.representation = "compile time do not transpile"
		} };
	}

	if (operand == "Vector") {
		if (expr.args.size() != 1)
			throw;
		expected<NodeStructs::MetaType> arg_t = std::visit(overload(
			[&](const std::string& e) -> expected<NodeStructs::MetaType> {
				return type_of_typename(state, variables, NodeStructs::BaseTypename{ e });
			},
			[&](const NodeStructs::Typename& t) -> expected<NodeStructs::MetaType> {
				return type_of_typename(state, variables, t);
			},
			[&](const NodeStructs::Expression& e) -> expected<NodeStructs::MetaType> {
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

		return expression_information{ type_information{
			.type = NodeStructs::MetaType{ NodeStructs::VectorType{.value_type = std::move(arg_t.value()) } },
			.representation = "Vector<" + repr.value() + ">"
		} };
	}

	return error{
		"user error",
		"not a template"
	};
}

std::optional<std::vector<non_type_information>> rearrange_if_possible(
	const auto& state,
	variables_t& variables,
	const std::vector<NodeStructs::MetaType>& v1,
	std::vector<non_type_information>&& v2,
	int i1,
	int i2,
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
	const std::vector<NodeStructs::MetaType>& v1,
	std::vector<non_type_information>&& v2
) {
	if (v1.size() != v2.size())
		return std::nullopt;
	bool no_rearrange_is_fine = true;
	for (int i = 0; i < v1.size(); ++i)
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
			throw;
		},
		[&](const NodeStructs::PrimitiveType& e) -> R {
			if (expr.arguments.args.size() != 1)
				return error{
					"user error",
					"expected 1 argument to initialize object of type `" + transpile_typename(state, variables, typename_of_primitive(e)).value() + "`"
			};
			auto arg_info = transpile_arg(state, variables, expr.arguments.args.at(0));
			return_if_error(arg_info);
			if (!std::holds_alternative<non_type_information>(arg_info.value()))
				throw;
			const auto& arg_info_ok = std::get<non_type_information>(arg_info.value());
			if (!std::holds_alternative<directly_assignable>(assigned_to(state, variables, { copy(e) }, arg_info_ok.type)._value))
				throw;
			return expression_information{ non_type_information{
				.type = { copy(e) },
				.representation = transpile_typename(state, variables, typename_of_primitive(e)).value() + "{ " + arg_info_ok.representation + " }",
				.value_category = NodeStructs::Value{},
			} };
		},
		[&](const NodeStructs::Type& tt) -> R {
			if (expr.arguments.args.size() != tt.member_variables.size())
				return error{
					"user error",
					"expected `" + std::to_string(tt.member_variables.size()) + "` arguments but received `" + std::to_string(expr.arguments.args.size()) + "` for type `" + tt.name + "`"
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
				throw;

			auto param_ts = vec_of_expected_to_expected_of_vec(
				tt.member_variables
				| std::views::transform([](const NodeStructs::MemberVariable& mem) -> const NodeStructs::Typename& { return mem.type; })
				| std::views::transform([&](auto&& tn) { return type_of_typename(state, variables, tn); })
				| to_vec()
			);
			return_if_error(param_ts);

			auto arranged = rearrange_if_possible(state, variables, param_ts.value(), std::move(non_type_args));
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
				.type = copy(tt),
				.representation = typename_repr.value() + "{" + args_repr.str() + "}",
				.value_category = NodeStructs::Value{},
			} };
		},
		[&](const NodeStructs::SetType& set_t) -> R {
			if (expr.arguments.args.size() != 0)
				throw;
			return expression_information{ non_type_information{
				.type = copy(set_t),
				.representation = typename_repr.value() + "{}",
				.value_category = NodeStructs::Value{},
			} };
		},
		[&](const NodeStructs::VectorType& vec_t) -> R {
			if (expr.arguments.args.size() != 0)
				return error{
					"user error",
					"vectors construction is done without arguments, expression was: `" + original_representation(expr.arguments.args[0].expr) + "`"
				};
			return expression_information{ non_type_information{
				.type = copy(vec_t),
				.representation = typename_repr.value() + "{}",
				.value_category = NodeStructs::Value{},
			} };
		},
		[&](const NodeStructs::UnionType& union_t) -> R {
			if (expr.arguments.args.size() < 1)
				return error{
					"user error",
					"unions require at least one argument, union type was `" + typename_of_type(state, union_t).value().rule_info.content + "`, expression was: `{}`"
				};
			if (expr.arguments.args.size() > 1)
				return error{
					"user error",
					"unions require at most one argument, union type was `" + typename_of_type(state, union_t).value().rule_info.content + "`, expression was: `TODO`"
				};
			auto expr_info = transpile_arg(state, variables, expr.arguments.args.at(0));
			return_if_error(expr_info);
			if (!std::holds_alternative<non_type_information>(expr_info.value()))
				throw;
			const non_type_information& expr_info_ok = std::get<non_type_information>(expr_info.value());
			for (const auto& T : union_t.arguments)
				if (cmp(T, expr_info_ok.type) == std::weak_ordering::equivalent)
					return expression_information{ non_type_information{
						.type = copy(union_t),
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
		throw;
	auto [type, repr, vcat] = std::get<non_type_information>(std::move(operand_info).value());
	return std::visit(overload(
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
				throw;
			const non_type_information& arg_info_ok = std::get<non_type_information>(arg_info.value());
			if (cmp(arg_info_ok.type, NodeStructs::MetaType{ NodeStructs::PrimitiveType{ NodeStructs::PrimitiveType::NonValued<int>{} } }) != std::weak_ordering::equivalent)
				throw;
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
				throw;
			const non_type_information& arg_info_ok = std::get<non_type_information>(arg_info.value());
			if (cmp(arg_info_ok.type, vt.key_type) != std::weak_ordering::equivalent)
				throw;
			return expression_information{ non_type_information{
				.type = std::move(vt).value_type,
				.representation = std::move(repr) + "[" + arg_info_ok.representation + "]",
				.value_category = std::move(vcat),
			} };
		}
	), std::move(type).type.get()._value);
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
		.arguments = std::move(joined)
	};
}

R T::operator()(const NodeStructs::PropertyAccessAndCallExpression& expr) {
	if (holds<NodeStructs::ParenArguments>(expr.operand)) {
		const auto& operand = get<NodeStructs::ParenArguments>(expr.operand);
		return operator()(rewire(expr, operand));
	}
	/*if (holds<NodeStructs::BraceArguments>(expr.operand)) {
		const auto& operand = std::get<NodeStructs::BraceArguments>(expr.operand);
		return operator()(rewire(expr, operand));
	}*/
	auto operand_info = transpile_expression(state, variables, expr.operand);
	return_if_error(operand_info);
	if (!std::holds_alternative<non_type_information>(operand_info.value()))
		throw;
	const non_type_information& operand_info_ok = std::get<non_type_information>(operand_info.value());
	return transpile_member_call(state, variables, operand_info_ok, expr.property_name, expr.arguments.args);
}

R T::operator()(const NodeStructs::PropertyAccessExpression& expr) {
	auto operand_info = transpile_expression(state, variables, expr.operand);
	return_if_error(operand_info);
	if (!std::holds_alternative<non_type_information>(operand_info.value()))
		throw;
	auto [type, repr, vcat] = std::get<non_type_information>(std::move(operand_info).value());

	auto t = type_of_postfix_member(state, variables, expr.property_name, type);
	return_if_error(t);

	if (std::holds_alternative<NodeStructs::InterfaceType>(type.type.get()._value))
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
		throw;
	auto inner = transpile_arg(state, variables, expr.args.at(0));
	return_if_error(inner);
	if (!std::holds_alternative<non_type_information>(inner.value()))
		throw;
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
		throw;
	auto args_ok = std::move(opt).value();

	std::stringstream ss;
	ss << "{ ";
	for (size_t i = 0; i < expr.args.size(); ++i)
		ss << args_ok.at(i).representation << ", ";
	ss << "}";
	return expression_information{ non_type_information{
		.type = NodeStructs::AggregateType{
			copy(expr.args),
			args_ok | std::views::transform([](non_type_information& e) { return std::move(e).type; }) | to_vec()
		},
		.representation = ss.str(),
		.value_category = NodeStructs::Value{}, // todo check conversion ok
	} };
}

R T::operator()(const std::string& expr) {
	if (auto it = variables.find(expr); it != variables.end() && it->second.size() > 0) {
		if (it->second.size() != 1)
			throw;
		const auto& v = it->second.back();
		return expression_information{ non_type_information{
			.type = copy(v.type),
			.representation = expr,
			.value_category = copy(v.value_category),
		} };
	}

	bool has_f = state.state.global_namespace.functions.find(expr) != state.state.global_namespace.functions.end();
	bool has_f_using_auto = state.state.global_namespace.functions_using_auto.find(expr) != state.state.global_namespace.functions_using_auto.end();
	if (has_f || has_f_using_auto)
		return expression_information{ type_information{
			.type = NodeStructs::MetaType{ NodeStructs::FunctionType{
				expr, state.state.global_namespace
			} },
			.representation = expr
		} };
	if (auto it = state.state.global_namespace.types.find(expr); it != state.state.global_namespace.types.end()) {
		if (it->second.size() != 1)
			throw;
		const auto& T = it->second.back();
		if (std::optional<error> err = traverse_type(state, T); err.has_value())
			return err.value();
		return expression_information{ type_information{
			.type = NodeStructs::MetaType{ copy(T) },
			.representation = expr
		} };
	}
	if (auto it = state.state.global_namespace.aliases.find(expr); it != state.state.global_namespace.aliases.end()) {
		const auto& type_name = it->second;
		auto type = type_of_typename(state, variables, type_name);
		return_if_error(type);
		if (std::optional<error> err = traverse_type(state, type.value()); err.has_value())
			return err.value();
		return expression_information{ type_information{
			.type = std::move(type).value(),
			.representation = transpile_typename(state, variables, type_name).value()
		} };
	}
	if (auto it = state.state.global_namespace.templates.find(expr); it != state.state.global_namespace.templates.end())
		return expression_information{ type_information{
			.type = NodeStructs::TemplateType{ expr, state.state.global_namespace },
			.representation = expr
		} };
	if (auto it = state.state.global_namespace.namespaces.find(expr); it != state.state.global_namespace.namespaces.end())
		return expression_information{ type_information{
			.type = NodeStructs::NamespaceType{ it->second },
			.representation = expr
		} };
	if (auto it = state.state.global_namespace.builtins.find(expr); it != state.state.global_namespace.builtins.end())
		return expression_information{ type_information{
			.type = it->second.back(),
			.representation = expr
		} };
	if (auto it = state.state.global_namespace.enums.find(expr); it != state.state.global_namespace.enums.end())
		return expression_information{ type_information{
			.type = NodeStructs::EnumType{ it->second.back() },
			.representation = expr
		} };
	return error{ "user error", "Undeclared identifier `" + expr + "`" };
}

R T::operator()(const Token<INTEGER_NUMBER>& expr) {
	return expression_information{ non_type_information{
		.type = NodeStructs::PrimitiveType{ NodeStructs::PrimitiveType::Valued<int>{ atoi(expr.value.c_str()) } },
		.representation = expr.value,
		.value_category = NodeStructs::Value{}
	} };
}

R T::operator()(const Token<FLOATING_POINT_NUMBER>& expr) {
	return expression_information{ non_type_information{
		.type = { NodeStructs::PrimitiveType{ NodeStructs::PrimitiveType::Valued<double>{ stod(expr.value) } } },
		.representation = expr.value,
		.value_category = NodeStructs::Value{}
	} };
}

R T::operator()(const Token<STRING>& expr) {
	if (expr.value.size() == 3) // char
		return expression_information{ non_type_information{
			.type = { NodeStructs::PrimitiveType{ NodeStructs::PrimitiveType::Valued<char>{ expr.value.at(1) } } },
			.representation = "Char{ '" + std::string{ expr.value.at(1) } + "' }",
			.value_category = NodeStructs::Value{}
		} };
	if (expr.value.size() == 4 && expr.value.at(1) == '\\') { // backslash character
		if (expr.value.at(2) == '"')
			return expression_information{ non_type_information{
				.type = { NodeStructs::PrimitiveType{ NodeStructs::PrimitiveType::Valued<char>{ expr.value.at(0) } } },
				.representation = "Char{ '" + std::string{ expr.value.at(2) } + "' }",
				.value_category = NodeStructs::Value{}
			} };
		else
			return expression_information{ non_type_information{
				.type = { NodeStructs::PrimitiveType{ NodeStructs::PrimitiveType::Valued<char>{ expr.value.at(0) } } },
				.representation = "Char{ '\\" + std::string{ expr.value.at(2) } + "' }",
				.value_category = NodeStructs::Value{}
			} };
	}
	else
		return expression_information{ non_type_information{
			.type = { NodeStructs::PrimitiveType{ NodeStructs::PrimitiveType::Valued<std::string>{ expr.value } } },
			.representation = "String{" + expr.value + "}",
			.value_category = NodeStructs::Value{}
		} };
}
