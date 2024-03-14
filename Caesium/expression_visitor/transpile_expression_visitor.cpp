#include "../core/toCPP.hpp"
#include "../utility/replace_all.hpp"
#include "../core/structurizer.hpp"
#include "../utility/vec_of_expected_to_expected_of_vec.hpp"
#include <algorithm>

using T = transpile_expression_visitor;
using R = T::R;

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
		const non_type_information& if_expr_info_ok = std::get<non_type_information>(condition_expr_info.value());

		auto else_expr_info = operator()(expr.ifElseExprs.value().second);
		return_if_error(else_expr_info);
		if (!std::holds_alternative<non_type_information>(else_expr_info.value()))
			throw;
		const non_type_information& else_expr_info_ok = std::get<non_type_information>(condition_expr_info.value());

		return expression_information{ non_type_information {
			.type = if_expr_info_ok.type,
			.representation = std::string("([&] () { if (") +
			condition_expr_info_ok.representation +
			") return " +
			if_expr_info_ok.representation +
			"; else return " +
			else_expr_info_ok.representation +
			"; }())",
			.value_category = if_expr_info_ok.value_category,
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
	ss << base_ok.representation;
	for (const auto& e : expr.ors) {
		auto or_expr = operator()(e);
		return_if_error(or_expr);
		if (!std::holds_alternative<non_type_information>(or_expr.value()))
			throw;
		const non_type_information& or_expr_ok = std::get<non_type_information>(or_expr.value());
		ss << " || " << or_expr_ok.representation;
	}
	return expression_information{ non_type_information {
		.type = *state.state.named.types.at("Bool").back(),
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
	ss << base_ok.representation;
	for (const auto& e : expr.ands) {
		auto and_ = operator()(e);
		return_if_error(and_);
		if (!std::holds_alternative<non_type_information>(and_.value()))
			throw;
		const non_type_information& and_ok = std::get<non_type_information>(and_.value());
		ss << " || " << and_ok.representation;
	}
	return expression_information{ non_type_information {
		.type = *state.state.named.types.at("Bool").back(),
		.representation = ss.str(),
		.value_category = NodeStructs::Value{},
	} };
}

R T::operator()(const NodeStructs::EqualityExpression& expr) {
	std::stringstream ss;
	auto base = operator()(expr.expr);
	return_if_error(base);
	if (!std::holds_alternative<non_type_information>(base.value()))
		throw;
	const non_type_information& base_ok = std::get<non_type_information>(base.value());
	ss << base_ok.representation;
	for (const auto& [op, e] : expr.equals) {
		auto eq = operator()(e);
		return_if_error(eq);
		if (std::holds_alternative<non_type_information>(eq.value())) {
			const non_type_information& eq_ok = std::get<non_type_information>(eq.value());
			ss << " " << symbol_variant_as_text(op) << " " << eq_ok.representation;
		}
		else
			throw;
	}
	return expression_information{ non_type_information {
		.type = *state.state.named.types.at("Bool").back(),
		.representation = ss.str(),
		.value_category = NodeStructs::Value{},
	} };
}

R T::operator()(const NodeStructs::CompareExpression& expr) {
	std::stringstream ss;
	auto base = operator()(expr.expr);
	return_if_error(base);
	if (!std::holds_alternative<non_type_information>(base.value()))
		throw;
	const non_type_information& base_ok = std::get<non_type_information>(base.value());
	ss << base_ok.representation;
	for (const auto& [op, e] : expr.comparisons) {
		auto cmp = operator()(e);
		return_if_error(cmp);
		if (!std::holds_alternative<non_type_information>(cmp.value()))
			throw;
		const non_type_information& cmp_ok = std::get<non_type_information>(cmp.value());
		ss << " " << symbol_variant_as_text(op) << " " << cmp_ok.representation;
	}
	return expression_information{ non_type_information {
		.type = *state.state.named.types.at("Bool").back(),
		.representation = ss.str(),
		.value_category = NodeStructs::Value{},
	} };
}

R T::operator()(const NodeStructs::AdditiveExpression& expr) {
	std::stringstream ss;
	auto base = operator()(expr.expr);
	return_if_error(base);
	if (std::holds_alternative<non_type_information>(base.value()))
		ss << std::get<non_type_information>(base.value()).representation;
	else
		throw;
	for (const auto& [op, e] : expr.adds) {
		auto add = operator()(e);
		return_if_error(add);
		if (std::holds_alternative<non_type_information>(add.value()))
			ss << " " << symbol_variant_as_text(op) << " " << std::get<non_type_information>(add.value()).representation;
		else
			throw;
	}
	return expression_information{ non_type_information {
		.type = *state.state.named.types.at("Int").back(),
		.representation = ss.str(),
		.value_category = NodeStructs::Value{},
	} };
}

R T::operator()(const NodeStructs::MultiplicativeExpression& expr) {
	std::stringstream ss;
	auto base = operator()(expr.expr);
	return_if_error(base);
	if (std::holds_alternative<non_type_information>(base.value()))
		ss << std::get<non_type_information>(base.value()).representation;
	else
		throw;
	for (const auto& [op, e] : expr.muls) {
		auto mul = operator()(e);
		return_if_error(mul);
		if (std::holds_alternative<non_type_information>(mul.value()))
			ss << " " << symbol_variant_as_text(op) << " " << std::get<non_type_information>(mul.value()).representation;
		else
			throw;
	}
	return expression_information{ non_type_information {
		.type = *state.state.named.types.at("Int").back(),
		.representation = ss.str(),
		.value_category = NodeStructs::Value{},
	} };
}

R T::operator()(const NodeStructs::UnaryExpression& expr) {
	std::stringstream ss;
	for (const auto& op : expr.unary_operators)
		ss << std::visit([&](const auto& token_expr) { return symbol_as_text(token_expr); }, op);
	auto base = operator()(expr.expr);
	return_if_error(base);
	if (!std::holds_alternative<non_type_information>(base.value()))
		throw;
	const non_type_information& base_ok = std::get<non_type_information>(base.value());
	ss << base_ok.representation;
	return expression_information{ non_type_information {
		.type = *state.state.named.types.at("Int").back(),
		.representation = ss.str(),
		.value_category = NodeStructs::Value{},
	} };
}

R T::operator()(const NodeStructs::CallExpression& expr) {
	if (std::holds_alternative<std::string>(expr.operand.expression.get())) {
		const std::string& operand = std::get<std::string>(expr.operand.expression.get());
		if (auto it = state.state.named.functions_using_auto.find(operand); it != state.state.named.functions_using_auto.end()) {
			std::vector<NodeStructs::FunctionArgument> args;
			auto fn = realise_function_using_auto(state, *it->second.back(), expr.arguments.args);
			return_if_error(fn);
			auto& vec = state.state.named.functions[fn.value().name];
			vec.push_back(new NodeStructs::Function(std::move(fn).value()));
		}
	}
	auto base = operator()(expr.operand);
	return_if_error(base);
	if (!std::holds_alternative<type_information>(base.value()))
		throw;
	const type_information& base_ok = std::get<type_information>(base.value());

	auto temp2 = type_of_function_like_call_with_args(state, expr.arguments.args, base_ok.type);
	return_if_error(temp2);
	return expression_information{ non_type_information {
		.type = std::move(temp2).value().second,
		.representation = transpile_call_expression_with_args(state, expr.arguments.args, expr.operand).value(),
		.value_category = temp2.value().first,
	} };
}

bool has_argument_and_forced_parameter_mismatch(const std::vector<NodeStructs::Expression>& args, const NodeStructs::Template& tmpl) {
	auto n_parameters = args.size();
	for (auto i = 0; i < n_parameters; ++i)
		if (tmpl.parameters.at(i).second.has_value() && (args.at(i) <=> tmpl.parameters.at(i).second.value() != std::strong_ordering::equal))
			return true;
	return false;
}

auto eliminate_argument_and_forced_parameter_mismatches(const std::vector<NodeStructs::Expression>& args, std::vector<NodeStructs::Template const*>&& templates) {
	for (int i = templates.size() - 1; i >= 0; --i)
		if (has_argument_and_forced_parameter_mismatch(args, *templates.at(i)))
			templates.erase(templates.begin() + i);
	return std::move(templates);
}

bool is_redundant_wrt_to(const NodeStructs::Template& tmpl, const NodeStructs::Template& reference) {
	auto n_parameters = tmpl.parameters.size();
	for (auto i = 0; i < n_parameters; ++i)
		if (!reference.parameters.at(i).second.has_value() && tmpl.parameters.at(i).second.has_value())
			return false;
	return true;
}

auto eliminate_underspecified(const std::vector<NodeStructs::Expression>& args, std::vector<NodeStructs::Template const*>&& templates) {
	auto mask = std::vector<bool>(templates.size(), false);
	for (int i = 0; i < templates.size(); ++i)
		for (int j = 0; j < templates.size(); ++j)
			if (i != j && mask[j] == false && is_redundant_wrt_to(*templates.at(i), *templates.at(j))) {
				mask[i] = true;
				break;
			}
	unsigned index = 0;
	templates.erase(std::remove_if(templates.begin(), templates.end(), [&](NodeStructs::Template const* t) {
		return bool(mask.at(index++));
		}), templates.end());
	return std::move(templates);
}

expected<std::reference_wrapper<const NodeStructs::Template>> find_best_template(
	const std::vector<NodeStructs::Template const*>& templates,
	const std::vector<NodeStructs::Expression>& args
) {
	auto n_parameters = args.size();
	for (unsigned i = 0; i < n_parameters; ++i) {
		const auto& parameter_name = templates.at(0)->parameters.at(i).first;
		for (auto const* a : templates)
			if (a->parameters.size() != n_parameters || parameter_name != a->parameters.at(i).first)
				throw;
	}

	auto candidates = templates;
	candidates = eliminate_argument_and_forced_parameter_mismatches(args, std::move(candidates));
	auto candidates2 = candidates;
	candidates = eliminate_underspecified(args, std::move(candidates));

	if (candidates.size() == 0)
		throw;
	else if (candidates.size() == 1)
		return std::reference_wrapper<const NodeStructs::Template>{ *candidates.at(0) };
	else {
		std::stringstream args_ss;
		bool has_prev_args = false;
		for (const auto& arg : args) {
			if (has_prev_args)
				args_ss << ", ";
			has_prev_args = true;
			args_ss << expression_for_template(arg);
		}
		std::stringstream templates_ss;
		bool has_prev_templates = false;
		for (auto const* a : candidates) {
			if (has_prev_templates)
				templates_ss << ", ";
			has_prev_templates = true;
			std::stringstream parameters_ss;
			bool has_prev_parameters = false;
			for (const auto& parameter : a->parameters) {
				if (has_prev_parameters)
					parameters_ss << ", ";
				has_prev_parameters = true;
				parameters_ss << parameter.first;
				if (parameter.second.has_value())
					parameters_ss << " = " << expression_for_template(parameter.second.value());
			}
			templates_ss << "template " + templates.at(0)->name + "<" + parameters_ss.str() + ">";
		}
		return error{
			"user error",
			"More than one instance of `" + templates.at(0)->name + "`"
			" matched the provided arguments [" + args_ss.str() + "]"
			", contenders were [" + templates_ss.str() + "]"
		};
	}
}

R T::operator()(const NodeStructs::TemplateExpression& expr) {
	if (!std::holds_alternative<std::string>(expr.operand.expression.get()))
		throw;

	if (auto it = state.state.named.templates.find(std::get<std::string>(expr.operand.expression.get())); it != state.state.named.templates.end()) {
		auto t = find_best_template(it->second, expr.arguments.args);
		return_if_error(t);
		const auto& tmpl = t.value().get();
		std::vector<std::string> args = expr.arguments.args | LIFT_TRANSFORM(expression_for_template) | to_vec();
		std::string tmpl_name = template_name(it->first, args);

		size_t max_params = it->second.at(0)->parameters.size();
		for (unsigned i = 1; i < it->second.size(); ++i)
			max_params = std::max(max_params, it->second.at(i)->parameters.size());

		if (tmpl.parameters.size() != expr.arguments.args.size()) {
			std::stringstream ss;
			ss << "invalid number of arguments to template `"
				<< tmpl.name
				<< "`, expected `"
				<< tmpl.parameters.size()
				<< "`, received `"
				<< expr.arguments.args.size()
				<< "`";
			return error{ "user error", ss.str() };
		}

		if (tmpl.templated == "BUILTIN") {
			/*if (tmpl.name == "Vector") {
				auto x = type_of_typename(state, templated_with.at(0));
				return_if_error(x);
				return NodeStructs::MetaType{ NodeStructs::VectorType{ Box<NodeStructs::MetaType>{ std::move(x).value() } } };
			}
			if (tmpl.name == "Set") {
				auto x = type_of_typename(state, templated_with.at(0));
				return_if_error(x);
				return NodeStructs::MetaType{ NodeStructs::SetType{ Box<NodeStructs::MetaType>{ std::move(x).value() } } };
			}
			if (tmpl.name == "Map") {
				auto x = type_of_typename(state, templated_with.at(0));
				auto y = type_of_typename(state, templated_with.at(1));
				return_if_error(x);
				return_if_error(y);
				return NodeStructs::MetaType{ NodeStructs::MapType{
					Box<NodeStructs::MetaType>{ std::move(x).value() },
					Box<NodeStructs::MetaType>{ std::move(y).value() }
				} };
			}*/
			throw;
		}

		std::string replaced = tmpl.templated;
		for (int i = 0; i < expr.arguments.args.size(); ++i)
			replaced = replace_all(std::move(replaced), tmpl.parameters.at(i).first, args.at(i));

		if (auto it = state.state.named.functions.find(tmpl_name); it != state.state.named.functions.end()) {
			return expression_information{ type_information {
				.type = NodeStructs::FunctionType{ *it->second.back() },
				.representation = template_name(tmpl.name, expr.arguments.args)
			} };
		}

		if (auto it = state.state.named.types.find(tmpl_name); it != state.state.named.types.end()) {
			return expression_information{ type_information {
				.type = NodeStructs::MetaType{ *it->second.back() },
				.representation = template_name(tmpl.name, expr.arguments.args)
			} };
		}

		{
			And<IndentToken, grammar::Function, Token<END>> f{ 1 };
			auto tokens = Tokenizer(replaced).read();
			tokens_and_iterator g{ tokens, tokens.begin() };
			if (!f.build(g.it))
				throw;
			auto* structured_f = new NodeStructs::Function{ getStruct(f.get<grammar::Function>(), std::nullopt) };
			structured_f->name = tmpl_name;
			state.state.named.functions[structured_f->name].push_back(structured_f);
			state.state.traversed_functions.insert(*structured_f);
			auto transpiled_or_e = transpile(state, *structured_f);
			return_if_error(transpiled_or_e);
			state.state.transpile_in_reverse_order.push_back(std::move(transpiled_or_e).value());
			return expression_information{ type_information {
				.type = NodeStructs::FunctionType{ *structured_f },
				.representation = template_name(tmpl.name, expr.arguments.args)
			} };
		}
		{
			And<IndentToken, grammar::Type, Token<END>> t{ 1 };
			auto tokens = Tokenizer(replaced).read();
			tokens_and_iterator g{ tokens, tokens.begin() };
			if (!t.build(g.it))
				throw;
			auto* structured_t = new NodeStructs::Type{ getStruct(t.get<grammar::Type>(), std::nullopt) };
			structured_t->name = tmpl_name;
			state.state.named.types[structured_t->name].push_back(structured_t);
			auto opt_error = traverse_type(state, *structured_t);
			if (opt_error.has_value())
				return opt_error.value();
			return expression_information{ type_information {
				.type = NodeStructs::MetaType{ *structured_t },
				.representation = template_name(tmpl.name, expr.arguments.args)
			} };
		}
		throw;
	}
	throw;
}

R T::operator()(const NodeStructs::ConstructExpression& expr) {
	auto t = type_of_typename(state, expr.operand);
	return_if_error(t);
	auto typename_repr = transpile_typename(state, expr.operand);
	return_if_error(typename_repr);
	return std::visit(overload(
		[&](const auto& e) -> R {
			throw;
		},
		[&](const std::reference_wrapper<const NodeStructs::Type>& tt) -> R {
			auto args_repr = transpile_args(state, expr.arguments.args);
			return_if_error(args_repr);

			return expression_information{ non_type_information {
				.type = tt,
				.representation = typename_repr.value() + "{" + args_repr.value() + "}",
				.value_category = NodeStructs::Value{},
			} };
		},
		[&](const NodeStructs::SetType& set_t) -> R {
			if (expr.arguments.args.size() != 0)
				throw;
			return expression_information{ non_type_information {
				.type = set_t,
				.representation = typename_repr.value() + "{}",
				.value_category = NodeStructs::Value{},
			} };
		},
		[&](const NodeStructs::VectorType& vec_t) -> R {
			if (expr.arguments.args.size() != 0)
				throw;
			return expression_information{ non_type_information {
				.type = vec_t,
				.representation = typename_repr.value() + "{}",
				.value_category = NodeStructs::Value{},
			} };
		},
		[&](const NodeStructs::UnionType& union_t) -> R {
			if (expr.arguments.args.size() < 1)
				return error{
					"user error",
					"unions require at least one argument"
				};
			if (expr.arguments.args.size() > 1)
				return error{
					"user error",
					"unions require at most one argument"
				};
			auto expr_info = operator()(expr.arguments.args.at(0).expr);
			return_if_error(expr_info);
			if (!std::holds_alternative<non_type_information>(expr_info.value()))
				throw;
			const non_type_information& expr_info_ok = std::get<non_type_information>(expr_info.value());
			for (const auto& T : union_t.arguments) {
				if (T <=> expr_info_ok.type.type == std::weak_ordering::equivalent)
					return expression_information{ non_type_information {
						.type = union_t,
						.representation = typename_repr.value() + "{" + expr_info_ok.representation + "}",
						.value_category = NodeStructs::Value{},
					} };
			}
			throw;
		}
	), t.value().type);
}

R T::operator()(const NodeStructs::BracketAccessExpression& expr) {
	auto operand_info = transpile_expression(state, expr.operand);
	return_if_error(operand_info);
	if (!std::holds_alternative<non_type_information>(operand_info.value()))
		throw;
	const non_type_information& operand_info_ok = std::get<non_type_information>(operand_info.value());
	/*if (std::holds_alternative<NodeStructs::VectorType>(operand_info_ok.type)) {
		throw;
		const auto& vt = std::get<NodeStructs::VectorType>(operand_info.value().type.value);
		if (expr.arguments.args.size() != 1)
			throw;
		auto arg_info = transpile_expression(state, expr.arguments.args.at(0).expr);
		return_if_error(arg_info);
		if (arg_info.value().type <=> NodeStructs::MetaType{ *state.state.named.types.at("Int").back() } != std::weak_ordering::equivalent)
			throw;
		return expression_information{
			.value_category = operand_info.value().value_category,
			.type = vt.value_type,
			.representation = operand_info.value().representation + "[" + arg_info.value().representation + "]"
		};
	}*/
	return error{
		"user error",
		"bracket access is reserved for vector, set and map types"
	};
}

R T::operator()(const NodeStructs::PropertyAccessAndCallExpression& expr) {
	if (std::holds_alternative<NodeStructs::ParenArguments>(expr.operand.expression.get())) {
		const auto& operand = std::get<NodeStructs::ParenArguments>(expr.operand.expression.get());
		auto joined = operand.args;
		for (const auto& arg : expr.arguments.args)
			joined.push_back(arg);
		auto rewired = NodeStructs::CallExpression{
			.operand = NodeStructs::Expression{ expr.property_name },
			.arguments = std::move(joined)
		};
		return operator()(rewired);
	}
	if (std::holds_alternative<NodeStructs::BraceArguments>(expr.operand.expression.get())) {
		const auto& operand = std::get<NodeStructs::BraceArguments>(expr.operand.expression.get());
		auto joined = operand.args;
		for (const auto& arg : expr.arguments.args)
			joined.push_back(arg);
		auto rewired = NodeStructs::CallExpression{
			.operand = NodeStructs::Expression{ expr.property_name },
			.arguments = std::move(joined)
		};
		return operator()(rewired);
	}
	auto operand_info = transpile_expression(state, expr.operand);
	return_if_error(operand_info);
	if (!std::holds_alternative<non_type_information>(operand_info.value()))
		throw;
	const non_type_information& operand_info_ok = std::get<non_type_information>(operand_info.value());
	
	return transpile_member_call(state, expr.operand, expr.property_name, expr.arguments.args, operand_info_ok.type.type);

	//auto t = type_of_postfix_member(state, expr.property_name, operand_t.value().second);
	//return_if_error(t);

	//if (std::holds_alternative<NodeStructs::BuiltInType>(t.value().second.value)) {
	//	const auto& builtin = std::get<NodeStructs::BuiltInType>(t.value().second.value);
	//	const auto& arguments = expr.arguments.args;
	//	return std::visit(
	//		overload(
	//			[&](const NodeStructs::BuiltInType::push_t& e) -> R {
	//				if (arguments.size() != 1)
	//					throw;
	//				auto arg_t = transpile_expression(state, std::get<NodeStructs::Expression>(arguments.at(0))).transform([](auto&& x) { return std::pair{ std::move(x).value_category, std::move(x).type }; });
	//				return_if_error(arg_t);
	//				if (!is_assignable_to(state, e.container.value_type.get(), arg_t.value().second)) {
	//					throw;
	//				}

	//				auto operand_repr = transpile_expression(state, expr.operand);
	//				return_if_error(operand_repr);
	//				auto args_repr = transpile_args(state, arguments);
	//				return_if_error(args_repr);
	//				return expression_information{
	//					.expression = expr,
	//					.value_category = NodeStructs::Value{},
	//					.type = *state.state.named.types.at("Void").back(),
	//					.representation = "push(" + operand_repr.value().representation + ", " + args_repr.value() + ")"
	//				};
	//			}
	//		),
	//		builtin.builtin
	//	);
	//}

	//if (std::holds_alternative<NodeStructs::FunctionType>(t.value().second.value)) {
	//	const auto& fn = std::get<NodeStructs::FunctionType>(t.value().second.value);
	//	const auto& unwrapped_fn = fn.function.get();
	//}

	//return error{
	//	"user error",
	//	"Use of type like a function is prohibited. Type was `" + transpile_type(state, t.value().second).value() + "`"
	//};
}

R T::operator()(const NodeStructs::PropertyAccessExpression& expr) {
	auto operand_info = transpile_expression(state, expr.operand);
	return_if_error(operand_info);
	if (!std::holds_alternative<non_type_information>(operand_info.value()))
		throw;
	const non_type_information& operand_info_ok = std::get<non_type_information>(operand_info.value());

	auto t = type_of_postfix_member(state, expr.property_name, operand_info_ok.type.type);
	return_if_error(t);

	if (std::holds_alternative<NodeStructs::InterfaceType>(operand_info_ok.type.type.type))
		return expression_information{ non_type_information {
			.type = t.value().second,
			.representation = "std::visit(overload([&](auto&& XX){ return XX." + expr.property_name + "; }), " + operand_info_ok.representation + ")",
			.value_category = operand_info_ok.value_category,
		} };
	else
		return expression_information{ non_type_information {
			.type = t.value().second,
			.representation = operand_info_ok.representation + "." + expr.property_name,
			.value_category = operand_info_ok.value_category,
		} };
}

R T::operator()(const NodeStructs::ParenArguments& expr) {
	if (expr.args.size() != 1)
		throw;
	auto inner = operator()(expr.args.at(0).expr);
	return_if_error(inner);
	if (!std::holds_alternative<non_type_information>(inner.value()))
		throw;
	const non_type_information& inner_ok = std::get<non_type_information>(inner.value());
	return expression_information{ non_type_information {
		.type = inner_ok.type,
		.representation = "(" + inner_ok.representation + ")",
		.value_category = argument_category_optional_to_value_category(expr.args.at(0).category), // todo check conversion ok
	} };
	 
	//throw;
	//auto transpiled_args = expr.args
	//	| LIFT_TRANSFORM_X(arg, std::pair{
	//		arg.category,
	//		operator()(arg.expr)
	//	})
	//	| to_vec();
	//for (const auto& arg : transpiled_args)
	//	return_if_error(arg.second);

	//auto x = transpiled_args
	//	| LIFT_TRANSFORM_X(cat_and_info, std::pair{
	//		cat_and_info.first.has_value() ? cat_and_info.first.value() : NodeStructs::ArgumentCategory{ NodeStructs::Move{} },
	//		cat_and_info.second.value().type
	//		})
	//	| to_vec();

	//auto args_repr = transpile_args(state, expr.args);
	//return_if_error(args_repr);

	//return expression_information{
	//	//.expression = expr,
	//	.value_category = NodeStructs::Value{},
	//	.type = NodeStructs::AggregateType{ x },
	//	.representation = "(" + args_repr.value() + ")"
	//};
}

R T::operator()(const NodeStructs::BraceArguments& expr) {
	throw;
	//auto transpiled_args = expr.args
	//	| LIFT_TRANSFORM_X(arg, std::pair{
	//		arg.category,
	//		operator()(arg.expr)
	//	})
	//	| to_vec();
	//for (const auto& arg : transpiled_args)
	//	return_if_error(arg.second);

	//auto x = transpiled_args
	//	| LIFT_TRANSFORM_X(cat_and_info, std::pair{
	//		cat_and_info.first.has_value() ? cat_and_info.first.value() : NodeStructs::ArgumentCategory{ NodeStructs::Move{} },
	//		cat_and_info.second.value().type
	//	})
	//	| to_vec();

	//auto args_repr = transpile_args(state, expr.args);
	//return_if_error(args_repr);

	//return expression_information{
	//	//.expression = expr,
	//	.value_category = NodeStructs::Value{},
	//	.type = NodeStructs::AggregateType{ x },
	//	.representation = "{" + args_repr.value() + "}"
	//};
}

R T::operator()(const std::string& expr) {
	if (auto it = state.state.variables.find(expr); it != state.state.variables.end() && it->second.size() > 0) {
		const auto& v = it->second.back();
		return expression_information{ non_type_information {
			.type = v.type,
			.representation = expr,
			.value_category = v.value_category,
		} };
	}
	if (auto it = state.state.named.functions.find(expr); it != state.state.named.functions.end()) {
		const auto& fn = *it->second.back();
		if (!state.state.traversed_functions.contains(fn)) {
			state.state.traversed_functions.insert(fn);
			auto t = transpile(state.unindented(), fn);
			return_if_error(t);
			state.state.transpile_in_reverse_order.push_back(std::move(t).value());
		}
		return expression_information{ type_information {
			.type = NodeStructs::FunctionType{ fn },
			.representation = expr
		} };
	}
	if (auto it = state.state.named.types.find(expr); it != state.state.named.types.end()) {
		const auto& T = *it->second.back();
		if (std::optional<error> err = traverse_type(state, T); err.has_value())
			return err.value();
		return expression_information{ type_information {
			.type = NodeStructs::MetaType{ T },
			.representation = expr
		} };
	}
	if (auto it = state.state.named.type_aliases.find(expr); it != state.state.named.type_aliases.end()) {
		const auto& alias = it->second;
		if (std::optional<error> err = traverse_type(state, alias); err.has_value())
			return err.value();
		return expression_information{ type_information {
			.type = alias,
			.representation = transpile_typename(state, state.state.named.type_aliases_typenames.at(expr)).value()
		} };
	}
	if (auto it = state.state.named.templates.find(expr); it != state.state.named.templates.end()) {
		const auto& tmpl = *it->second.back();
		return expression_information{ type_information {
			.type = NodeStructs::Template{ tmpl },
			.representation = expr
		} };
	}
	return error{ "user error", "Undeclared variable `" + expr + "`" };
}

R T::operator()(const Token<INTEGER_NUMBER>& expr) {
	
	return expression_information{ non_type_information {
		.type = { *state.state.named.types.at("Int").back() },
		.representation = expr.value,
		.value_category = NodeStructs::Value{}
	} };
}

R T::operator()(const Token<FLOATING_POINT_NUMBER>& expr) {
	return expression_information{ non_type_information {
		.type = { *state.state.named.types.at("Floating").back() },
		.representation = expr.value,
		.value_category = NodeStructs::Value{}
	} };
}

R T::operator()(const Token<STRING>& expr) {
	return expression_information{ non_type_information {
		.type = { *state.state.named.types.at("String").back() },
		.representation = expr.value,
		.value_category = NodeStructs::Value{}
	} };
}
