#include "../core/toCPP.hpp"
#include "../utility/replace_all.hpp"
#include "../core/structurizer.hpp"
#include "../utility/vec_of_expected_to_expected_of_vec.hpp"
#include "../utility/vec_of_variant_to_optional_vector_single_type.hpp"
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
		non_type_information& if_expr_info_ok = std::get<non_type_information>(condition_expr_info.value());

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
		.type = NodeStructs::PrimitiveType{ { bool{} } },
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
		.type = NodeStructs::PrimitiveType{ { bool{} } },
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
			ss << " " << symbol_variant_as_text(op._value) << " " << eq_ok.representation;
		}
		else
			throw;
	}
	return expression_information{ non_type_information{
		.type = NodeStructs::PrimitiveType{ { bool{} } },
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
		ss << " " << symbol_variant_as_text(op._value) << " " << cmp_ok.representation;
	}
	return expression_information{ non_type_information{
		.type = NodeStructs::PrimitiveType{ { bool{} } },
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
			ss << " " << symbol_variant_as_text(op._value) << " " << std::get<non_type_information>(add.value()).representation;
		else
			throw;
	}
	return expression_information{ non_type_information{
		.type = std::move(std::get<non_type_information>(base.value()).type.type),
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
			ss << " " << symbol_variant_as_text(op._value) << " " << std::get<non_type_information>(mul.value()).representation;
		else
			throw;
	}

	return expression_information{ non_type_information{
		.type = NodeStructs::PrimitiveType{ { int{} } },
		.representation = ss.str(),
		.value_category = NodeStructs::Value{},
	} };
}

R T::operator()(const NodeStructs::UnaryExpression& expr) {
	std::stringstream ss;
	for (const auto& op : expr.unary_operators)
		ss << symbol_variant_as_text(op._value);
	auto base = operator()(expr.expr);
	return_if_error(base);
	if (!std::holds_alternative<non_type_information>(base.value()))
		throw;
	const non_type_information& base_ok = std::get<non_type_information>(base.value());
	ss << base_ok.representation;
	return expression_information{ non_type_information{
		.type = NodeStructs::PrimitiveType{ { int{} } },
		.representation = ss.str(),
		.value_category = NodeStructs::Value{},
	} };
}

R T::operator()(const NodeStructs::CallExpression& expr) {
	if (std::holds_alternative<std::string>(expr.operand.expression.get()._value)) {
		const std::string& operand = std::get<std::string>(expr.operand.expression.get()._value);
		if (auto it = state.state.global_namespace.functions_using_auto.find(operand); it != state.state.global_namespace.functions_using_auto.end()) {
			std::vector<NodeStructs::FunctionArgument> args;
			auto fn = realise_function_using_auto(state, it->second.back(), expr.arguments.args);
			return_if_error(fn);
			auto& vec = state.state.global_namespace.functions[fn.value().name];
			bool found = false;
			for (const auto& f : vec)
				if (f <=> fn.value() == std::weak_ordering::equivalent)
					found = true;
			if (!found)
				vec.push_back(std::move(fn).value());
		}
	}
	auto t = transpile_expression(state, expr.operand);
	return_if_error(t);
	if (std::holds_alternative<type_information>(t.value())) {
		const auto& ti = std::get<type_information>(t.value());
		if (std::holds_alternative<NodeStructs::Builtin>(ti.type.type._value)) {
			const std::string& name = std::get<NodeStructs::Builtin>(ti.type.type._value).name;
			if (name == "print" || name == "println") {
				std::stringstream ss;

				ss << "(Void)(std::cout";
				for (const auto& [_, arg] : expr.arguments.args) {
					auto expr_s = expr_to_printable(state, arg);
					return_if_error(expr_s);
					ss << " << " << expr_s.value();
				}
				bool newline = name == "println";
				if (newline)
					ss << " << \"\\n\"";
				ss << ")";
				return expression_information{ non_type_information{
					.type = NodeStructs::PrimitiveType{ { NodeStructs::void_t{} } },
					.representation = ss.str(),
					.value_category = NodeStructs::Value{}
				} };
			}
			throw;
		}
		if (std::holds_alternative<NodeStructs::TemplateType>(ti.type.type._value)) {
			const auto& options = std::get<NodeStructs::TemplateType>(ti.type.type._value)
				.name_space.get()
				.templates.at(std::get<NodeStructs::TemplateType>(ti.type.type._value).name);
			if (options.size() != 1)
				throw;
			const auto& tmpl = options.back();
			/*auto arg_ts = vec_of_expected_to_expected_of_vec(
				expr.arguments.args
				| LIFT_TRANSFORM_TRAIL(.expr)
				| LIFT_TRANSFORM_X(X, transpile_expression(state, X))
				| to_vec()
			);*/
			throw;
		}
		if (std::holds_alternative<NodeStructs::FunctionType>(ti.type.type._value)) {
			auto expected_f = find_best_function(
				state,
				std::get<NodeStructs::FunctionType>(ti.type.type._value).name,
				expr.arguments.args,
				std::get<NodeStructs::FunctionType>(ti.type.type._value).name_space
			);
			return_if_error(expected_f);
			if (!expected_f.value().has_value())
				return error{ "user error", "no matching function" };
			const NodeStructs::Function& fn = *expected_f.value().value();
			if (!state.state.traversed_functions.contains(fn)) {
				// TODO VERIFY
				if (uses_auto(fn))
					throw;
				state.state.traversed_functions.insert(copy(fn));
				state.state.functions_to_transpile.insert(copy(fn));
			}
			auto args_or_error = transpile_args(state, expr.arguments.args);
			return_if_error(args_or_error);
			return expression_information{ non_type_information{
				.type = type_of_typename(state, fn.returnType).value(),
				.representation = ti.representation + "(" + std::move(args_or_error).value() + ")",
				.value_category = NodeStructs::Value{}
			} };
		}
		throw;
	}
	throw;
}

R T::operator()(const NodeStructs::NamespaceExpression& expr) {
	transpile_t2 operand_t = operator()(expr.name_space);
	return_if_error(operand_t);
	if (!std::holds_alternative<type_information>(operand_t.value()))
		throw;
	const auto& operand_t_ok = std::get<type_information>(operand_t.value());
	return std::visit(overload(
		[&](const NodeStructs::NamespaceType& nst) -> transpile_t2 {
			const auto& ns = nst.name_space.get();
			if (auto it = ns.types.find(expr.name_in_name_space); it != ns.types.end())
				return expression_information{ type_information{
					.type = copy(it->second.back()),
					.representation = operand_t_ok.representation + "__" + expr.name_in_name_space
				} };
			if (auto it = ns.functions.find(expr.name_in_name_space); it != ns.functions.end())
				throw;
			return error{
				"user error",
				"namespace `" + ns.name + "` has no type or function `" + expr.name_in_name_space + "`"
			};
		},
		[&](const NodeStructs::EnumType& enumt) -> transpile_t2 {
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
		[&](const auto& nst) -> transpile_t2 {
			throw;
		}
	), operand_t_ok.type.type._value);
}

R T::operator()(const NodeStructs::TemplateExpression& expr) {
	auto test = operator()(expr.operand);
	return_if_error(test);
	if (!std::holds_alternative<type_information>(test.value()))
		throw;
	const auto& test_ok = std::get<type_information>(test.value());
	if (!std::holds_alternative<NodeStructs::TemplateType>(test_ok.type.type._value))
		throw;

	if (!std::holds_alternative<std::string>(expr.operand.expression.get()._value))
		throw;

	if (auto it = state.state.global_namespace.templates.find(std::get<std::string>(expr.operand.expression.get()._value));
		it != state.state.global_namespace.templates.end()) {
		auto t = find_best_template(it->second, expr.arguments.args);
		return_if_error(t);
		const auto& tmpl = t.value().tmpl.get();
		std::vector<std::string> args = expr.arguments.args | LIFT_TRANSFORM(expression_for_template) | to_vec();
		std::string tmpl_name = template_name(it->first, expr.arguments.args);

		size_t max_params = it->second.at(0).parameters.size();
		for (unsigned i = 1; i < it->second.size(); ++i)
			max_params = std::max(max_params, it->second.at(i).parameters.size());

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
			And<IndentToken, grammar::Function, Token<END>> f{ 1 };
			auto tokens = Tokenizer(replaced).read();
			tokens_and_iterator g{ tokens, tokens.begin() };
			if (build(f, g.it)) {
				auto structured_f = getStruct(f.get<grammar::Function>(), std::nullopt);
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
					// TODO VERIFY
					state.state.traversed_functions.insert(copy(structured_f));
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
		{
			And<IndentToken, grammar::Type, Token<END>> t{ 1 };
			auto tokens = Tokenizer(replaced).read();
			tokens_and_iterator g{ tokens, tokens.begin() };
			if (build(t, g.it)) {
				auto structured_t = NodeStructs::Type{ getStruct(t.get<grammar::Type>(), std::nullopt) };
				structured_t.name = tmpl_name;
				state.state.global_namespace.types[structured_t.name].push_back(copy(structured_t));
				auto opt_error = traverse_type(state, structured_t);
				if (opt_error.has_value())
					return opt_error.value();
				return expression_information{ type_information{
					.type = NodeStructs::MetaType{ std::move(structured_t) },
					.representation = template_name(tmpl.name, expr.arguments.args)
				} };
			}
		}
		return error{
			"user error",
			"Template expansion does not result in a type or function: |begin|\n" + replaced + "\n|end|"
		};
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
	throw;
	/*if (i1 == v1.size())
		return mappings | LIFT_TRANSFORM(v2.at) | to_vec();*/
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
		[&](const NodeStructs::PrimitiveType& e) -> R {
			if (expr.arguments.args.size() != 1)
				return error{
					"user error",
					"expected 1 argument to initialize object of type `" + transpile_typename(state, typename_of_primitive(e)).value() + "`"
			};
			auto arg_info = operator()(expr.arguments.args.at(0).expr);
			return_if_error(arg_info);
			if (!std::holds_alternative<non_type_information>(arg_info.value()))
				throw;
			const auto& arg_info_ok = std::get<non_type_information>(arg_info.value());
			if (!is_assignable_to(state, { copy(e) }, arg_info_ok.type.type))
				throw;
			return expression_information{ non_type_information{
				.type = { copy(e) },
				.representation = transpile_typename(state, typename_of_primitive(e)).value() + "{ " + arg_info_ok.representation + " }",
				.value_category = NodeStructs::Value{},
			} };
		},
		[&](const NodeStructs::Type& tt) -> R {
			if (expr.arguments.args.size() != tt.member_variables.size())
				return error{
					"user error",
					"expected `" + std::to_string(tt.member_variables.size()) + "` arguments but received `" + std::to_string(expr.arguments.args.size()) + "`"
				};
			throw;
			/*auto arg_ts = vec_of_expected_to_expected_of_vec(expr.arguments.args | LIFT_TRANSFORM_X(arg, operator()(arg.expr)) | to_vec());
			return_if_error(arg_ts);
			const auto& arg_ts_ok = arg_ts.value();*/
			throw;
			/*auto non_type_args = arg_ts_ok | filter_transform_variant_type_eq(non_type_information) | to_vec();
			if (non_type_args.size() != arg_ts_ok.size())
				throw;*/

			throw;
			/*auto param_ts = vec_of_expected_to_expected_of_vec(
				tt.member_variables | LIFT_TRANSFORM_TRAIL(.type) | LIFT_TRANSFORM_X(tn, type_of_typename(state, tn)) | to_vec()
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
			} };*/
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
				throw;
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
						.type = copy(union_t),
						.representation = typename_repr.value() + "{" + expr_info_ok.representation + "}",
						.value_category = NodeStructs::Value{},
					} };
			}
			return error{
				"user error",
				"expression does not match any of the union type"
			};
		}
	), t.value().type._value);
}

R T::operator()(const NodeStructs::BracketAccessExpression& expr) {
	auto operand_info = transpile_expression(state, expr.operand);
	return_if_error(operand_info);
	if (!std::holds_alternative<non_type_information>(operand_info.value()))
		throw;
	auto [type, repr, vcat] = std::get<non_type_information>(std::move(operand_info).value());
	return std::visit(overload(
		[&](const auto&) -> R {
			return error{
				"user error",
				"bracket access is reserved for vector, set and map types"
			};
		},
		[&](NodeStructs::VectorType&& vt) -> R {
			auto arg_info = transpile_expression(state, expr.arguments.args.at(0).expr);
			return_if_error(arg_info);
			if (!std::holds_alternative<non_type_information>(arg_info.value()))
				throw;
			const non_type_information& arg_info_ok = std::get<non_type_information>(arg_info.value());
			if (arg_info_ok.type.type <=> NodeStructs::MetaType{ NodeStructs::PrimitiveType{ { int{} } } } != std::weak_ordering::equivalent)
				throw;
			return expression_information{ non_type_information{
				.type = std::move(vt).value_type,
				.representation = std::move(repr) + "[" + arg_info_ok.representation + "]",
				.value_category = std::move(vcat),
			} };
		},
		[&](NodeStructs::MapType&& vt) -> R {
			auto arg_info = transpile_expression(state, expr.arguments.args.at(0).expr);
			return_if_error(arg_info);
			if (!std::holds_alternative<non_type_information>(arg_info.value()))
				throw;
			const non_type_information& arg_info_ok = std::get<non_type_information>(arg_info.value());
			if (arg_info_ok.type.type <=> vt.key_type.get() != std::weak_ordering::equivalent)
				throw;
			return expression_information{ non_type_information{
				.type = std::move(vt).value_type,
				.representation = std::move(repr) + "[" + arg_info_ok.representation + "]",
				.value_category = std::move(vcat),
			} };
		}
	), std::move(type).type.type._value);
}

R T::operator()(const NodeStructs::PropertyAccessAndCallExpression& expr) {
	if (std::holds_alternative<NodeStructs::ParenArguments>(expr.operand.expression.get()._value)) {
		const auto& operand = std::get<NodeStructs::ParenArguments>(expr.operand.expression.get()._value);
		std::vector<NodeStructs::FunctionArgument> joined;
		for (const auto& arg : operand.args)
			joined.push_back(copy(arg));
		for (const auto& arg : expr.arguments.args)
			joined.push_back(copy(arg));
		auto rewired = NodeStructs::CallExpression{
			.operand = NodeStructs::Expression{ expr.property_name },
			.arguments = std::move(joined)
		};
		return operator()(rewired);
	}
	if (std::holds_alternative<NodeStructs::BraceArguments>(expr.operand.expression.get()._value)) {
		const auto& operand = std::get<NodeStructs::BraceArguments>(expr.operand.expression.get()._value);
		std::vector<NodeStructs::FunctionArgument> joined;
		for (const auto& arg : operand.args)
			joined.push_back(copy(arg));
		for (const auto& arg : expr.arguments.args)
			joined.push_back(copy(arg));
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

	return transpile_member_call(state, operand_info_ok, expr.property_name, expr.arguments.args);
}

R T::operator()(const NodeStructs::PropertyAccessExpression& expr) {
	auto operand_info = transpile_expression(state, expr.operand);
	return_if_error(operand_info);
	if (!std::holds_alternative<non_type_information>(operand_info.value()))
		throw;
	auto [type, repr, vcat] = std::get<non_type_information>(std::move(operand_info).value());

	auto t = type_of_postfix_member(state, expr.property_name, type.type);
	return_if_error(t);

	if (std::holds_alternative<NodeStructs::InterfaceType>(type.type.type._value))
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
	auto inner = operator()(expr.args.at(0).expr);
	return_if_error(inner);
	if (!std::holds_alternative<non_type_information>(inner.value()))
		throw;
	non_type_information inner_ok = std::get<non_type_information>(std::move(inner).value());
	return expression_information{ non_type_information{
		.type = std::move(inner_ok).type,
		.representation = "(" + inner_ok.representation + ")",
		.value_category = argument_category_optional_to_value_category(expr.args.at(0).category), // todo check conversion ok
	} };
}

R T::operator()(const NodeStructs::BraceArguments& expr) {
	auto vec = expr.args | LIFT_TRANSFORM_X(arg, transpile_expression(state, FORWARD(arg).expr)) | to_vec();
	auto args_ = vec_of_expected_to_expected_of_vec(std::move(vec));
	return_if_error(args_);
	auto args_ok_maybe_wrong_type = std::move(args_).value();
	auto opt = vec_of_variant_to_optional_vector_single_type<non_type_information>(std::move(args_ok_maybe_wrong_type));
	if (!opt.has_value())
		throw;
	auto args_ok = std::move(opt).value();

	NodeStructs::AggregateType res;
	std::stringstream ss;
	ss << "{ ";
	for (size_t i = 0; i < expr.args.size(); ++i) {
		auto arg = std::move(args_ok.at(i));
		ss << arg.representation << ", ";
		res.arguments.push_back(std::pair{
			expr.args.at(i).category.has_value() ?
				copy(expr.args.at(i).category.value()) :
				NodeStructs::ArgumentCategory{ NodeStructs::Reference{} },// todo args_ok.at(i).category
			std::move(arg).type.type
		});
	}
	ss << "}";
	return expression_information{ non_type_information{
		.type = std::move(res),
		.representation = ss.str(),
		.value_category = NodeStructs::Value{}, // todo check conversion ok
	} };
}

R T::operator()(const std::string& expr) {
	if (auto it = state.state.variables.find(expr); it != state.state.variables.end() && it->second.size() > 0) {
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
	throw;
	/*if (auto it = state.state.global_namespace.aliases.find(expr); it != state.state.global_namespace.aliases.end()) {
		const auto& type_name = it->second;
		auto type = type_of_typename(state, type_name);
		return_if_error(type);
		if (std::optional<error> err = traverse_type(state, type.value()); err.has_value())
			return err.value();
		return expression_information{ type_information{
			.type = std::move(type).value(),
			.representation = transpile_typename(state, type_name).value()
		} };
	}*/
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
	return error{ "user error", "Undeclared variable `" + expr + "`" };
}

R T::operator()(const Token<INTEGER_NUMBER>& expr) {
	return expression_information{ non_type_information{
		.type = NodeStructs::PrimitiveType{ { int{} } },
		.representation = expr.value,
		.value_category = NodeStructs::Value{}
	} };
}

R T::operator()(const Token<FLOATING_POINT_NUMBER>& expr) {
	return expression_information{ non_type_information{
		.type = { NodeStructs::PrimitiveType{ { double{} } } },
		.representation = expr.value,
		.value_category = NodeStructs::Value{}
	} };
}

R T::operator()(const Token<STRING>& expr) {
	return expression_information{ non_type_information{
		.type = { NodeStructs::PrimitiveType{ { std::string{} } } },
		.representation = "String{" + expr.value + "}",
		.value_category = NodeStructs::Value{}
	} };
}
