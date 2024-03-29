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

		return expression_information{ non_type_information{
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
	return expression_information{ non_type_information{
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
	return expression_information{ non_type_information{
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
	return expression_information{ non_type_information{
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
	return expression_information{ non_type_information{
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
	return expression_information{ non_type_information{
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
	return expression_information{ non_type_information{
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
	return expression_information{ non_type_information{
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
	auto t = transpile_expression(state, expr.operand);
	return_if_error(t);
	if (std::holds_alternative<type_information>(t.value())) {
		const auto& ti = std::get<type_information>(t.value());
		if (std::holds_alternative<NodeStructs::TemplateType>(ti.type.type)) {
			const auto& options = std::get<NodeStructs::TemplateType>(ti.type.type).options;
			if (options.size() != 1)
				throw;
			const auto& tmpl = *options.back();
			auto arg_ts = vec_of_expected_to_expected_of_vec(
				expr.arguments.args
				| LIFT_TRANSFORM_TRAIL(.expr)
				| LIFT_TRANSFORM_X(X, transpile_expression(state, X))
				| to_vec()
			);
			if (tmpl.templated == "BUILTIN") {
				if (tmpl.name == "print" || tmpl.name == "println") {
					std::stringstream ss;

					ss << "(Void)(std::cout";
					for (const auto& [_, arg] : expr.arguments.args) {
						auto expr_s = expr_to_printable(state, arg);
						return_if_error(expr_s);
						ss << " << " << expr_s.value();
					}
					bool newline = tmpl.name == "println";
					if (newline)
						ss << " << \"\\n\"";
					ss << ")";
					return expression_information{ non_type_information{
						.type = *state.state.named.types.at("Void").back(),
						.representation = ss.str(),
						.value_category = NodeStructs::Value{}
					} };
				}
				throw;
			}
			throw;
		}
		if (std::holds_alternative<NodeStructs::FunctionType>(ti.type.type)) {
			const auto& fn = std::get<NodeStructs::FunctionType>(ti.type.type).function.get();
			if (!state.state.traversed_functions.contains(fn)) {
				state.state.traversed_functions.insert(fn);
				auto t = transpile(state.unindented(), fn);
				return_if_error(t);
				state.state.transpile_in_reverse_order.push_back(std::move(t).value());
			}
			auto args_or_error = transpile_args(state, expr.arguments.args);
			return_if_error(args_or_error);
			return expression_information{ non_type_information{
				.type = type_of_typename(state, fn.returnType).value(),
				.representation = std::move(ti).representation + "(" + std::move(args_or_error).value() + ")",
				.value_category = NodeStructs::Value{}
			} };
		}
		throw;
	}
	throw;
}

R T::operator()(const NodeStructs::NamespaceExpression& expr) {
	using pair_t = std::pair<std::string, std::reference_wrapper<const NodeStructs::NameSpace>>;
	auto ns_or_e_f = overload(
		[&](this auto&& ns_or_e_f, const NodeStructs::NamespaceExpression& nse) -> expected<pair_t> {
			expected<pair_t> ns_or_e = ns_or_e_f(nse.name_space);
			return_if_error(ns_or_e);
			const NodeStructs::NameSpace& ns = ns_or_e.value().second.get();
			const std::string& ns_repr = ns_or_e.value().first;
			if (
				auto it = std::find_if(ns.namespaces.begin(), ns.namespaces.end(), [&](const auto& n) { return n.name == nse.name_in_name_space; });
				it != ns.namespaces.end()
			)
				return pair_t{ ns_repr + "__" + nse.name_in_name_space, *it };
			return error{
				"user error",
				"namespace `" + ns.name + "` has no namespace `" + nse.name_in_name_space + "`"
			};
		},
		[&](const std::string& ns) -> expected<pair_t> {
			if (auto it = state.state.named.namespaces.find(ns); it != state.state.named.namespaces.end())
				return pair_t{ ns, *it->second.back() };
			else
				return error{
					"user error",
					"`" + ns + "` is not a namespace"
			};
		},
		[&](this auto&& ns_or_e_f, const NodeStructs::Expression& other) -> expected<pair_t> {
			return std::visit(ns_or_e_f, other.expression.get());
		},
		[&](const auto& other) -> expected<pair_t> {
			auto e = transpile_expression(state, other);
			return_if_error(e);
			return error{
				"user error",
				"`" + std::visit([&](const auto& x) { return x.representation; }, e.value()) + "` is not a namespace"
			};
		}
	);
	expected<pair_t> ns_or_e = ns_or_e_f(expr.name_space);
	return_if_error(ns_or_e);
	const NodeStructs::NameSpace& ns = ns_or_e.value().second.get();
	const std::string& ns_repr = ns_or_e.value().first;
	if (
		auto it = std::find_if(ns.types.begin(), ns.types.end(), [&](const auto& t) { return t.name == expr.name_in_name_space; });
		it != ns.types.end()
	)
		return expression_information{ type_information{
			.type = *it,
			.representation = ns_repr + "__" + expr.name_in_name_space
		} };
	if (
		auto it = std::find_if(ns.functions.begin(), ns.functions.end(), [&](const auto& f) { return f.name == expr.name_in_name_space; });
		it != ns.functions.end()
	)
		return expression_information{ type_information{
			.type = NodeStructs::FunctionType{ *it },
			.representation = ns_repr + "__" + expr.name_in_name_space
		} };
	return error{
		"user error",
		"namespace `" + ns.name + "` has no type or function `" + expr.name_in_name_space + "`"
	};
}

R T::operator()(const NodeStructs::TemplateExpression& expr) {
	auto test = operator()(expr.operand);
	return_if_error(test);
	if (!std::holds_alternative<type_information>(test.value()))
		throw;
	const auto& test_ok = std::get<type_information>(test.value());
	if (!std::holds_alternative<NodeStructs::TemplateType>(test_ok.type.type))
		throw;

	if (!std::holds_alternative<std::string>(expr.operand.expression.get()))
		throw;

	if (auto it = state.state.named.templates.find(std::get<std::string>(expr.operand.expression.get())); it != state.state.named.templates.end()) {
		auto t = find_best_template(it->second, expr.arguments.args);
		return_if_error(t);
		const auto& tmpl = t.value().tmpl.get();
		std::vector<std::string> args = expr.arguments.args | LIFT_TRANSFORM(expression_for_template) | to_vec();
		std::string tmpl_name = template_name(it->first, args);

		size_t max_params = it->second.at(0)->parameters.size();
		for (unsigned i = 1; i < it->second.size(); ++i)
			max_params = std::max(max_params, it->second.at(i)->parameters.size());

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

		const auto& arg_placements = t.value().arg_placements;
		std::string replaced = tmpl.templated;
		auto grab_nth_with_commas = [&](size_t i) {
			std::stringstream ss;
			bool has_previous = false;
			for (size_t j = 0; j < arg_placements.size(); ++j) {
				size_t k = arg_placements.at(j);
				if (k == i) {
					const auto& arg = args.at(j);
					if (has_previous)
						ss << ", ";
					has_previous = true;
					ss << arg;
				}
			}
			return ss.str();
		};
		for (size_t i = 0; i < tmpl.parameters.size(); ++i) {
			replaced = replace_all(
				std::move(replaced),
				std::visit(overload(
					[](const auto& e) { return e.name; },
					[](const NodeStructs::VariadicTemplateParameter& e) { return e.name + "..."; }
				), tmpl.parameters.at(i)),
				grab_nth_with_commas(i)
			);
		}

		if (auto it = state.state.named.functions.find(tmpl_name); it != state.state.named.functions.end()) {
			return expression_information{ type_information{
				.type = NodeStructs::FunctionType{ *it->second.back() },
				.representation = template_name(tmpl.name, expr.arguments.args)
			} };
		}

		if (auto it = state.state.named.types.find(tmpl_name); it != state.state.named.types.end()) {
			return expression_information{ type_information{
				.type = NodeStructs::MetaType{ *it->second.back() },
				.representation = template_name(tmpl.name, expr.arguments.args)
			} };
		}

		{
			And<IndentToken, grammar::Function, Token<END>> f{ 1 };
			auto tokens = Tokenizer(replaced).read();
			tokens_and_iterator g{ tokens, tokens.begin() };
			if (build(f, g.it)) {
				auto* structured_f = new NodeStructs::Function{ getStruct(f.get<grammar::Function>(), std::nullopt) };
				structured_f->name = tmpl_name;
				state.state.named.functions[structured_f->name].push_back(structured_f);
				state.state.traversed_functions.insert(*structured_f);
				auto transpiled_or_e = transpile(state, *structured_f);
				return_if_error(transpiled_or_e);
				state.state.transpile_in_reverse_order.push_back(std::move(transpiled_or_e).value());
				return expression_information{ type_information{
					.type = NodeStructs::FunctionType{ *structured_f },
					.representation = template_name(tmpl.name, expr.arguments.args)
				} };
			}
		}
		{
			And<IndentToken, grammar::Type, Token<END>> t{ 1 };
			auto tokens = Tokenizer(replaced).read();
			tokens_and_iterator g{ tokens, tokens.begin() };
			if (build(t, g.it)) {
				auto* structured_t = new NodeStructs::Type{ getStruct(t.get<grammar::Type>(), std::nullopt) };
				structured_t->name = tmpl_name;
				state.state.named.types[structured_t->name].push_back(structured_t);
				auto opt_error = traverse_type(state, *structured_t);
				if (opt_error.has_value())
					return opt_error.value();
				return expression_information{ type_information{
					.type = NodeStructs::MetaType{ *structured_t },
					.representation = template_name(tmpl.name, expr.arguments.args)
				} };
			}
		}
		throw;
	}
	return error{
		"user error",
		"not a template"
	};
}

std::optional<std::vector<non_type_information>> rearrange_if_possible(
	const std::vector<NodeStructs::MetaType>& v1,
	std::vector<non_type_information>&& v2,
	int i1,
	int i2,
	std::vector<int>&& mappings
) {
	if (i1 == v1.size())
		return mappings | LIFT_TRANSFORM(v2.at) | to_vec();
	if (i2 == v2.size())
		return std::nullopt;
	if (v1.at(i1) <=> v2.at(i2).type.type == std::weak_ordering::equivalent) {
		mappings.push_back(i2);
		return rearrange_if_possible(v1, std::move(v2), i1 + 1, 0, std::move(mappings));
	}
	else
		return rearrange_if_possible(v1, std::move(v2), i1, i2 + 1, std::move(mappings));
}


std::optional<std::vector<non_type_information>> rearrange_if_possible(
	const auto& state,
	const std::vector<NodeStructs::MetaType>& v1,
	std::vector<non_type_information>&& v2
) {
	if (v1.size() != v2.size())
		return std::nullopt;
	bool no_rearrange_is_fine = true;
	for (int i = 0; i < v1.size(); ++i)
		if (!is_assignable_to(state, v1.at(i), v2.at(i).type.type)) {
			no_rearrange_is_fine = false;
			break;
		}
	if (no_rearrange_is_fine)
		return std::move(v2);
	else
		return rearrange_if_possible(v1, std::move(v2), 0, 0, {});
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
			if (expr.arguments.args.size() != tt.get().member_variables.size())
				return error{
					"user error",
					"expected `" + std::to_string(tt.get().member_variables.size()) + "` arguments but received `" + std::to_string(expr.arguments.args.size()) + "`"
				};
			auto arg_ts = vec_of_expected_to_expected_of_vec(expr.arguments.args | LIFT_TRANSFORM_X(arg, operator()(arg.expr)) | to_vec());
			return_if_error(arg_ts);
			const auto& arg_ts_ok = arg_ts.value();
			auto non_type_args = arg_ts_ok | filter_transform_variant_type_eq(non_type_information) | to_vec();
			if (non_type_args.size() != arg_ts_ok.size())
				throw;

			auto param_ts = vec_of_expected_to_expected_of_vec(
				tt.get().member_variables | LIFT_TRANSFORM_TRAIL(.type) | LIFT_TRANSFORM_X(tn, type_of_typename(state, tn)) | to_vec()
			);
			return_if_error(param_ts);

			auto arranged = rearrange_if_possible(state, param_ts.value(), std::move(non_type_args));
			if (!arranged)
				throw;

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
				.type = tt,
				.representation = typename_repr.value() + "{" + args_repr.str() + "}",
				.value_category = NodeStructs::Value{},
			} };
		},
		[&](const NodeStructs::SetType& set_t) -> R {
			if (expr.arguments.args.size() != 0)
				throw;
			return expression_information{ non_type_information{
				.type = set_t,
				.representation = typename_repr.value() + "{}",
				.value_category = NodeStructs::Value{},
			} };
		},
		[&](const NodeStructs::VectorType& vec_t) -> R {
			if (expr.arguments.args.size() != 0)
				throw;
			return expression_information{ non_type_information{
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
					return expression_information{ non_type_information{
						.type = union_t,
						.representation = typename_repr.value() + "{" + expr_info_ok.representation + "}",
						.value_category = NodeStructs::Value{},
					} };
			}
			return error{
				"user error",
				"expression does not match any of the union type"
			};
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
		return expression_information{ non_type_information{
			.type = t.value().second,
			.representation = "std::visit(overload([&](auto&& XX){ return XX." + expr.property_name + "; }), " + operand_info_ok.representation + ")",
			.value_category = operand_info_ok.value_category,
		} };
	else
		return expression_information{ non_type_information{
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
	return expression_information{ non_type_information{
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
		return expression_information{ non_type_information{
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
		return expression_information{ type_information{
			.type = NodeStructs::FunctionType{ fn },
			.representation = expr
		} };
	}
	if (auto it = state.state.named.types.find(expr); it != state.state.named.types.end()) {
		const auto& T = *it->second.back();
		if (std::optional<error> err = traverse_type(state, T); err.has_value())
			return err.value();
		return expression_information{ type_information{
			.type = NodeStructs::MetaType{ T },
			.representation = expr
		} };
	}
	if (auto it = state.state.named.type_aliases_typenames.find(expr); it != state.state.named.type_aliases_typenames.end()) {
		const auto& type_name = it->second;
		auto type = type_of_typename(state, type_name);
		return_if_error(type);
		if (std::optional<error> err = traverse_type(state, type.value()); err.has_value())
			return err.value();
		return expression_information{ type_information{
			.type = type.value(),
			.representation = transpile_typename(state, state.state.named.type_aliases_typenames.at(expr)).value()
		} };
	}
	if (auto it = state.state.named.templates.find(expr); it != state.state.named.templates.end()) {
		return expression_information{ type_information{
			.type = NodeStructs::TemplateType{ expr, it->second },
			.representation = expr
		} };
	}
	if (auto it = state.state.named.namespaces.find(expr); it != state.state.named.namespaces.end()) {
		const auto& ns = *it->second.back();
		return expression_information{ type_information{
			.type = NodeStructs::NamespaceType{ ns },
			.representation = expr
		} };
	}
	return error{ "user error", "Undeclared variable `" + expr + "`" };
}

R T::operator()(const Token<INTEGER_NUMBER>& expr) {
	
	return expression_information{ non_type_information{
		.type = { *state.state.named.types.at("Int").back() },
		.representation = expr.value,
		.value_category = NodeStructs::Value{}
	} };
}

R T::operator()(const Token<FLOATING_POINT_NUMBER>& expr) {
	return expression_information{ non_type_information{
		.type = { *state.state.named.types.at("Floating").back() },
		.representation = expr.value,
		.value_category = NodeStructs::Value{}
	} };
}

R T::operator()(const Token<STRING>& expr) {
	return expression_information{ non_type_information{
		.type = { *state.state.named.types.at("String").back() },
		.representation = "String{" + expr.value + "}",
		.value_category = NodeStructs::Value{}
	} };
}
