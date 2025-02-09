#include "../core/toCPP.hpp"

using R = transpile_t;

template <typename context, bool is_compile_time>
R f(
	transpilation_state_with_indent state,
	variables_t& variables,
	const NodeStructs::VariableDeclarationStatement<context>& statement,
	const NodeStructs::MetaType& type,
	const std::string& type_repr,
	non_type_information assigned_expression_ok
) {
	if constexpr (is_compile_time) {
		if (holds<NodeStructs::CompileTimeType>(type))
			variables[statement.name].push_back({ NodeStructs::MutableReference{}, copy(assigned_expression_ok.type) });
		else if (holds<NodeStructs::PrimitiveType>(type))
			variables[statement.name].push_back({ NodeStructs::MutableReference{}, NodeStructs::CompileTimeType{ copy(assigned_expression_ok.type) } });
		else
			throw;
		return "";
	}
	else {
		variables[statement.name].push_back({ NodeStructs::MutableReference{}, copy(type) });

		if (holds<NodeStructs::UnionType>(assigned_expression_ok.type)
			&& (cmp(type, assigned_expression_ok.type) != std::weak_ordering::equivalent)) {
			std::string lambda_var_name = [&]() { std::stringstream ss; ss << "auto" << state.state.current_variable_unique_id++; return ss.str(); }();
			return type_repr + " " + statement.name + " = "
				"std::visit([](const auto& " + lambda_var_name + ") -> " + type_repr + " {"
				" return " + lambda_var_name + "; "
				"}, " + assigned_expression_ok.representation + ");\n";
		}
		else {
			if (cmp(assigned_expression_ok.type, type) == std::weak_ordering::equivalent)
				return type_repr + " " + statement.name + " = " + assigned_expression_ok.representation + ";\n";
			else
				return type_repr + " " + statement.name + " = { " + assigned_expression_ok.representation + " };\n";
		}
	}
}

template <typename context, bool is_compile_time>
R transpile_statement_specific(
	transpilation_state_with_indent state,
	variables_t& variables,
	const NodeStructs::MetaType& expected_return_type,
	const NodeStructs::Expression& statement
) {
	auto repr =  transpile_expression(state, variables, statement);
	return_if_error(repr);
	if (!std::holds_alternative<non_type_information>(repr.value()))
		throw;
	const auto& repr_ok = std::get<non_type_information>(repr.value());
	return repr_ok.representation + ";\n";
}

template <typename context, bool is_compile_time>
R transpile_statement_specific(
	transpilation_state_with_indent state,
	variables_t& variables,
	const NodeStructs::MetaType& expected_return_type,
	const NodeStructs::VariableDeclarationStatement<context>& statement
) {
	bool is_aggregate_init = holds<NodeStructs::BraceArguments>(statement.expr);
	bool is_auto = cmp(statement.type.value, make_typename(NodeStructs::BaseTypename{ "auto" }, std::nullopt, rule_info_language_element("auto")).value) == std::weak_ordering::equivalent;
	if (is_auto) {
		if (is_aggregate_init)
			return error{
				"user error",
				"auto cannot deduce aggregate initialization"
			};

		auto assigned_expression = transpile_expression(state, variables, statement.expr);
		return_if_error(assigned_expression);
		if (std::holds_alternative<non_type_information>(assigned_expression.value())) {
			auto assigned_expression_ok = std::get<non_type_information>(std::move(assigned_expression).value());

			auto deduced_typename = typename_of_type(state, assigned_expression_ok.type);
			return_if_error(deduced_typename);

			auto deduced_typename_repr = transpile_typename(state, variables, deduced_typename.value());
			return_if_error(deduced_typename_repr);

			variables[statement.name].push_back({ NodeStructs::MutableReference{}, std::move(assigned_expression_ok).type });

			return deduced_typename_repr.value() + " " + statement.name + " = " + assigned_expression_ok.representation + ";\n";
		}
		throw;
	}
	else {
		auto type = type_of_typename(state, variables, statement.type);
		return_if_error(type);

		auto tn = typename_of_type(state, type.value());
		return_if_error(tn);

		auto type_repr = transpile_typename(state, variables, tn.value());
		return_if_error(type_repr);

		auto assigned_expression = [&]() {
			if (is_aggregate_init) {
				const auto& aggregate = get<NodeStructs::BraceArguments>(statement.expr);
				auto as_construct = NodeStructs::ConstructExpression{
					.operand = copy(tn.value()),
					.arguments = copy(aggregate.args)
				};
				return transpile_expression(state, variables, as_construct);
			}
			else
				return transpile_expression(state, variables, statement.expr);
		}();
		return_if_error(assigned_expression);
		if (!std::holds_alternative<non_type_information>(assigned_expression.value()))
			throw;

		auto& assigned_expression_ok = std::get<non_type_information>(assigned_expression.value());

		auto assignable = assigned_to(state, variables, type.value(), assigned_expression_ok.type)._value;
		return std::visit(overload(
			[&](const directly_assignable& assignable) -> R {
				return f<context, is_compile_time>(state, variables, statement, type.value(), type_repr.value(), std::move(assigned_expression_ok));
			},
			[&](requires_conversion& assignable) -> R {
				transpile_expression_information_t converted = assignable.converter(state, variables, statement.expr);
				return_if_error(converted);
				if (!std::holds_alternative<non_type_information>(converted.value()))
					throw;
				return f<context, is_compile_time>(state, variables, statement, type.value(), type_repr.value(), std::get<non_type_information>(std::move(converted).value()));
			},
			[&](const not_assignable&) -> R {
				return error{
					"user error",
					"expression not assignable to type, type was `"
					+ original_representation(statement.type)
					+ "`, expression was `"
					+ original_representation(statement.expr)
					+ "`"
				};
			}
		), assignable);
	}
}

template <typename context, bool is_compile_time>
R transpile_statement_specific(
	transpilation_state_with_indent state,
	variables_t& variables,
	const NodeStructs::MetaType& expected_return_type,
	const NodeStructs::IfStatement<context>& statement
) {
	if constexpr (is_compile_time) {
		auto cnd = transpile_expression(state, variables, statement.ifExpr);
		return_if_error(cnd);
		if (!std::holds_alternative<non_type_information>(cnd.value()))
			throw;
		const auto& cnd_ok = std::get<non_type_information>(cnd.value());

		bool cnd_value = [&]() {
			if (holds<NodeStructs::PrimitiveType>(cnd_ok.type)
				&& holds<NodeStructs::PrimitiveType::Valued<bool>>(get<NodeStructs::PrimitiveType>(cnd_ok.type).value))
					return get<NodeStructs::PrimitiveType::Valued<bool>>(get<NodeStructs::PrimitiveType>(cnd_ok.type).value).value;
			if (!holds<NodeStructs::CompileTimeType>(cnd_ok.type))
				throw;
			if (!holds<NodeStructs::PrimitiveType>(get<NodeStructs::CompileTimeType>(cnd_ok.type).type))
				throw;
			if (!holds<NodeStructs::PrimitiveType::Valued<bool>>(get<NodeStructs::PrimitiveType>(get<NodeStructs::CompileTimeType>(cnd_ok.type).type).value))
				throw;
			return get<NodeStructs::PrimitiveType::Valued<bool>>(get<NodeStructs::PrimitiveType>(get<NodeStructs::CompileTimeType>(cnd_ok.type).type).value).value;
		}();


		if (cnd_value) {
			auto if_statements = transpile(state.indented(), variables, statement.ifStatements, expected_return_type);
			return_if_error(if_statements);
			return if_statements.value();
		}
		else if (statement.elseExprStatements.has_value()) {
			throw;
		}
		else
			return "";
	}
	else {

		auto if_statements = transpile(state.indented(), variables, statement.ifStatements, expected_return_type);
		return_if_error(if_statements);

		auto if_expr = transpile_expression(state, variables, statement.ifExpr);
		return_if_error(if_expr);
		if (!std::holds_alternative<non_type_information>(if_expr.value()))
			throw;
		const auto& if_expr_ok = std::get<non_type_information>(if_expr.value());

		if (statement.elseExprStatements.has_value())
			return "if (" +
			if_expr_ok.representation +
			") {\n" +
			if_statements.value() +
			indent(state.indent) + "} else " +
			std::visit(
				overload(overload_default_error,
					[&](const NonCopyableBox<NodeStructs::IfStatement<context>>& elseif) {
						return transpile_statement_specific<context, is_compile_time>(state, variables, expected_return_type, elseif.get()).value();
					},
					[&](const std::vector<NodeStructs::Statement<context>>& justelse) {
						return "{\n" + transpile(state.indented(), variables, justelse, expected_return_type).value() + indent(state.indent) + "}\n";
					}
				),
				statement.elseExprStatements.value()._value
			);
		else {
			auto k = transpile_expression(state, variables, statement.ifExpr);
			return_if_error(k);
			if (!std::holds_alternative<non_type_information>(k.value()))
				throw;
			const auto& k_ok = std::get<non_type_information>(k.value());
			return "if (" +
				k_ok.representation +
				") {\n" +
				if_statements.value() +
				indent(state.indent) +
				"}\n";
		}
	}
}

struct empty_tag {};

template <bool ifor, bool is_compile_time>
R transpile_for_or_ifor_statement(
	transpilation_state_with_indent state,
	variables_t& variables,
	const NodeStructs::MetaType& expected_return_type,
	const auto& statement,
	const auto& index_or_none
) {
	if constexpr (is_compile_time) {
		auto coll_type_or_e = transpile_expression(state, variables, statement.collection);
		return_if_error(coll_type_or_e);
		if (!std::holds_alternative<type_information>(coll_type_or_e.value()))
			throw;
		const auto& coll_type_or_e_ok = std::get<type_information>(coll_type_or_e.value());
		if (holds<NodeStructs::TypeListType>(coll_type_or_e_ok.type)) {
			const NodeStructs::TypeListType& tl = get<NodeStructs::TypeListType>(coll_type_or_e_ok.type);
			if (statement.iterators.size() != 1 || !std::holds_alternative<std::string>(statement.iterators[0]._value))
				throw;
			const std::string& type_iterator = std::get<std::string>(statement.iterators[0]._value);
			
			int i = 0;
			std::stringstream ss;
			for (const NodeStructs::MetaType& type : tl.types) {
				auto tn = typename_of_type(state, type);
				return_if_error(tn);
				if constexpr (ifor) {
					const std::string& index = index_or_none;
					variables[index].push_back(variable_info{
						NodeStructs::ValueCategory{ NodeStructs::Value{} },
						NodeStructs::MetaType{ NodeStructs::CompileTimeType{
							.type = NodeStructs::MetaType{ NodeStructs::PrimitiveType{ NodeStructs::PrimitiveType::Valued<int>{ i } } }
						} }
					});
					i += 1;
				}
				state.state.global_namespace.aliases.insert({ type_iterator, std::move(tn).value() });
				auto statements = transpile(state, variables, statement.statements, expected_return_type);
				if constexpr (ifor) {
					const std::string& index = index_or_none;
					variables[index].pop_back();
				}
				return_if_error(statements);
				ss << statements.value();
			}
			state.state.global_namespace.aliases.erase(type_iterator);
			return ss.str();
		}
		throw;
	}
	else {
		auto coll_type_or_e = transpile_expression(state, variables, statement.collection);
		return_if_error(coll_type_or_e);
		if (!std::holds_alternative<non_type_information>(coll_type_or_e.value()))
			throw;
		const auto& coll_type_or_e_ok = std::get<non_type_information>(coll_type_or_e.value());
		auto it_type = iterator_type(state, coll_type_or_e_ok.type);

		std::stringstream ss;
		if (statement.iterators.size() > 1)
			throw;
		auto opt_e = add_for_iterator_variables(state, variables, statement.iterators, it_type);
		if (opt_e.has_value())
			return opt_e.value();
		if constexpr (ifor) {
			ss << "int " << index_or_none << " = -1;\n";
		}
		ss << "for (auto&& " << std::visit(overload(overload_default_error,
			[&](const NodeStructs::VariableDeclaration& iterator) {
				return iterator.name;
			},
			[&](const std::string& iterator) {
				return iterator;
			}
		), statement.iterators.at(0)._value);

		auto s1 = transpile_expression(state, variables, statement.collection);
		return_if_error(s1);
		if (!std::holds_alternative<non_type_information>(s1.value()))
			throw;
		const auto& s1_ok = std::get<non_type_information>(s1.value());
		auto s2 = transpile(state.indented(), variables, statement.statements, expected_return_type);
		return_if_error(s2);

		ss << " : "
			<< s1_ok.representation
			<< ") {\n";
		if constexpr (ifor) {
			ss << indent(state.indent)
				<< index_or_none << " += 1;\n";
		}
		ss << s2.value()
			<< indent(state.indent)
			<< "}\n";
		return ss.str();
	}
}

template <typename context, bool is_compile_time>
R transpile_statement_specific(
	transpilation_state_with_indent state,
	variables_t& variables,
	const NodeStructs::MetaType& expected_return_type,
	const NodeStructs::ForStatement<context>& statement
) {
	return transpile_for_or_ifor_statement<false, is_compile_time>(state, variables, expected_return_type, statement, empty_tag{});
}

template <typename context, bool is_compile_time>
R transpile_statement_specific(
	transpilation_state_with_indent state,
	variables_t& variables,
	const NodeStructs::MetaType& expected_return_type,
	const NodeStructs::IForStatement<context>& statement
) {
	return transpile_for_or_ifor_statement<true, is_compile_time>(state, variables, expected_return_type, statement.for_statement, statement.index_iterator);
}

template <typename context, bool is_compile_time>
R transpile_statement_specific(
	transpilation_state_with_indent state,
	variables_t& variables,
	const NodeStructs::MetaType& expected_return_type,
	const NodeStructs::WhileStatement<context>& statement
) {
	auto while_expr_info = transpile_expression(state, variables, statement.whileExpr);
	return_if_error(while_expr_info);
	if (!std::holds_alternative<non_type_information>(while_expr_info.value()))
		throw;
	const auto& while_expr_info_ok = std::get<non_type_information>(while_expr_info.value());
	auto transpiled_statements = transpile(state.indented(), variables, statement.statements, expected_return_type);
	return_if_error(transpiled_statements);
	return "while (" + while_expr_info_ok.representation + ") {\n" + transpiled_statements.value() + indent(state.indent) + "}\n";
}

template <typename context, bool is_compile_time>
R transpile_statement_specific(
	transpilation_state_with_indent state,
	variables_t& variables,
	const NodeStructs::MetaType& expected_return_type,
	const NodeStructs::BreakStatement<context>& statement
) {
	if (!statement.ifExpr.has_value())
		return "break;\n";
	auto s1 = transpile_expression(state, variables, statement.ifExpr.value());
	return_if_error(s1);
	if (!std::holds_alternative<non_type_information>(s1.value()))
		throw;
	const auto& s1_ok = std::get<non_type_information>(s1.value());
	return "if (" + s1_ok.representation + ") break;\n";
}

template <typename context, bool is_compile_time>
R transpile_statement_specific(
	transpilation_state_with_indent state,
	variables_t& variables,
	const NodeStructs::MetaType& expected_return_type,
	const NodeStructs::ReturnStatement<context>& statement
) {
	if (statement.returnExpr.size() == 0)
		return "return;\n";
	if (statement.returnExpr.size() != 1)
		throw; // gonna have to see about that
	R return_expression = [&]() -> R {
		auto expr_info = transpile_expression(state, variables, statement.returnExpr.at(0).expr);
		return_if_error(expr_info);
		if (!std::holds_alternative<non_type_information>(expr_info.value()))
			throw;
		const auto& expr_info_ok = std::get<non_type_information>(expr_info.value());
		auto assign = assigned_to(state, variables, expected_return_type, expr_info_ok.type)._value;
		if (std::holds_alternative<not_assignable>(assign)) {
			auto assign2 = assigned_to(state, variables, expected_return_type, expr_info_ok.type)._value;
			return error{
				"user error",
				"returned expression does not match declared return type, expression was `"
				+ original_representation(statement.returnExpr.at(0).expr)
				+ "`, type was `" + original_representation(typename_of_type(state, expected_return_type).value()) + "`"
			};
		}
		else if (std::holds_alternative<directly_assignable>(assign)) {
			return expr_info_ok.representation;
		}
		else {
			auto& rc =  std::get<requires_conversion>(assign);
			auto converted = rc.converter(state, variables, statement.returnExpr.at(0).expr);
			return_if_error(converted);
			if (!std::holds_alternative<non_type_information>(converted.value()))
				throw;
			const auto& converted_ok = std::get<non_type_information>(converted.value());
			return converted_ok.representation;
		}
	}();
	return_if_error(return_expression);
	if (statement.ifExpr.has_value()) {
		auto cnd = transpile_expression(state, variables, statement.ifExpr.value());
		return_if_error(cnd);
		if (!std::holds_alternative<non_type_information>(cnd.value()))
			throw;
		const auto& cnd_ok = std::get<non_type_information>(cnd.value());
		return "if (" + cnd_ok.representation + ") return " + return_expression.value() + ";\n";
	}
	else
		return "return " + return_expression.value() + ";\n";
}

template <typename context, bool is_compile_time>
R transpile_statement_specific(
	transpilation_state_with_indent state,
	variables_t& variables,
	const NodeStructs::MetaType& expected_return_type,
	const NodeStructs::BlockStatement<context>& statement
) {
	const auto& s = get<NodeStructs::BaseTypename>(statement.parametrized_block).type;
	if (state.state.global_namespace.blocks.contains(s)) {
		const NodeStructs::Block& block = state.state.global_namespace.blocks.at(s).back();
		std::stringstream ss;
		for (const auto& statement_in_block : block.statements)
			ss << transpile_statement(state, variables, expected_return_type, statement_in_block).value();
		return ss.str();
	}
	else
		return error{ "user error", "bad block name" + s };
}

template <typename context, bool is_compile_time>
R transpile_statement_specific(
	transpilation_state_with_indent state,
	variables_t& variables,
	const NodeStructs::MetaType& expected_return_type,
	const NodeStructs::MatchStatement<context>& statement
) {
	std::stringstream ss;
	if (statement.expressions.size() != 1)
		throw;
	if (statement.cases.size() == 0)
		throw;
	auto expr_info = transpile_expression(state, variables, statement.expressions.at(0));
	return_if_error(expr_info);
	if (!std::holds_alternative<non_type_information>(expr_info.value()))
		throw;
	const auto& expr_ok = std::get<non_type_information>(expr_info.value());
	auto tn = typename_of_type(state, expr_ok.type);
	return_if_error(tn);
	auto tn_repr = transpile_typename(state, variables, tn.value());
	return_if_error(tn_repr);
	unsigned id = state.state.current_variable_unique_id++;
	ss << "const " << tn_repr.value() << "& matchval" << id << " = " << expr_ok.representation << ";\n";
	for (const NodeStructs::MatchCase<context>& match_case : statement.cases) {
		auto tn = transpile_typename(state, variables, match_case.variable_declarations.at(0).first);
		return_if_error(tn);
		const auto& varname = match_case.variable_declarations.at(0).second;

		variables[varname]
			.push_back(variable_info{
				.value_category = NodeStructs::Reference{},
				.type = type_of_typename(state, variables, match_case.variable_declarations.at(0).first).value()
			});
		auto statements = transpile(state.indented(), variables, match_case.statements, expected_return_type);
		variables[varname].pop_back();
		return_if_error(statements);

		ss  << indent(state.indent)
			<< "if (std::holds_alternative<"
			<< tn.value()
			<< ">(matchval" << id << ")) {\n"
			<< indent(state.indent + 1)
			<< "const " << tn.value() << "& " << varname << " = std::get<"
			<< tn.value()
			<< ">(matchval" << id << ");\n"
			<< statements.value()
			<< indent(state.indent)<< "} else\n";
	}
	// here we would actually know at compile time that this cant be hit so we wouldnt actually insert a throw, it will be removed eventually
	ss << indent(state.indent + 1) << "throw;\n";
	return ss.str();
}

bool is_int(const auto& t) {
	return cmp(t, NodeStructs::MetaType{ NodeStructs::PrimitiveType{ NodeStructs::PrimitiveType::NonValued<int>{} } }) == std::weak_ordering::equivalent;
}

bool is_floating(const auto& t) {
	return cmp(t, NodeStructs::MetaType{ NodeStructs::PrimitiveType{ NodeStructs::PrimitiveType::NonValued<double>{} } }) == std::weak_ordering::equivalent;
}

bool is_char(const auto& t) {
	return cmp(t, NodeStructs::MetaType{ NodeStructs::PrimitiveType{ NodeStructs::PrimitiveType::NonValued<char>{} } }) == std::weak_ordering::equivalent;
}

bool is_string(const auto& t) {
	return cmp(t, NodeStructs::MetaType{ NodeStructs::PrimitiveType{ NodeStructs::PrimitiveType::NonValued<std::string>{} } }) == std::weak_ordering::equivalent;
}

template <typename T>
std::string case_fix(std::string case_expr);

template <>
std::string case_fix<Token<INTEGER_NUMBER>>(std::string case_expr) {
	return case_expr;
}

template <>
std::string case_fix<Token<STRING>>(std::string case_expr) {
	if (case_expr == "\"\\\"\"") return "'\"'";  // "\"" -> '"'
	if (case_expr == "\"'\"")    return "'\\''"; // "'"  -> '\''
	case_expr[0] = '\'';
	case_expr[case_expr.length() - 1] = '\'';
	return case_expr;
}

template <typename token_string_or_token_int, typename context>
R int_or_char_switch(
	transpilation_state_with_indent state,
	variables_t& variables,
	const NodeStructs::MetaType& expected_return_type,
	const NodeStructs::SwitchStatement<context>& statement,
	const non_type_information& expr_info_ok
) {
	std::stringstream ss;
	ss << "switch (" << expr_info_ok.representation << ") {\n";
	for (const NodeStructs::SwitchCase<context>& switch_case : statement.cases) {
		const auto& [switch_expr, switch_statements] = switch_case;
		if (!std::holds_alternative<token_string_or_token_int>(switch_expr.expression.get()._value))
			throw;
		const auto& val = std::get<token_string_or_token_int>(switch_expr.expression.get()._value);
		ss << indent(state.indent + 1) << "case " << case_fix<token_string_or_token_int>(val.value) << ":\n";
		if (switch_statements.size() > 0) {
			auto statements = transpile(state.indented().indented(), variables, switch_statements, expected_return_type);
			return_if_error(statements);
			ss << statements.value();
		}
	}
	ss << indent(state.indent) << "}\n";
	return ss.str();
}

template <typename context, bool is_compile_time>
R transpile_statement_specific(
	transpilation_state_with_indent state,
	variables_t& variables,
	const NodeStructs::MetaType& expected_return_type,
	const NodeStructs::SwitchStatement<context>& statement
) {
	auto expr_info = transpile_expression(state, variables, statement.expr);
	return_if_error(expr_info);
	if (!std::holds_alternative<non_type_information>(expr_info.value()))
		throw;
	const auto& expr_info_ok = std::get<non_type_information>(expr_info.value());
	if (is_int(expr_info_ok.type)) {
		return int_or_char_switch<Token<INTEGER_NUMBER>, context>(state, variables, expected_return_type, statement, expr_info_ok);
	}
	if (is_char(expr_info_ok.type)) {
		return int_or_char_switch<Token<STRING>, context>(state, variables, expected_return_type, statement, expr_info_ok);
	}
	if (is_floating(expr_info_ok.type)) {
		throw;
	}
	if (is_string(expr_info_ok.type)) {
		throw;
	}
	throw;
}

template <typename context, bool is_compile_time>
R transpile_statement_specific(
	transpilation_state_with_indent state,
	variables_t& variables,
	const NodeStructs::MetaType& expected_return_type,
	const NodeStructs::Assignment<context>& statement
) {
	if constexpr (is_compile_time) {
		if (!holds<std::string>(statement.left))
			throw;
		const std::string& var = get<std::string>(statement.left);
		auto right = transpile_expression(state, variables, statement.right);
		return_if_error(right);
		if (!std::holds_alternative<non_type_information>(right.value()))
			throw;
		if (holds<NodeStructs::CompileTimeType>(std::get<non_type_information>(right.value()).type)) {
			if (variables.at(var).size() != 1)
				throw;
			variables.at(var).pop_back();
			variables.at(var).push_back(variable_info{ .value_category = NodeStructs::Value{}, .type = std::move(get<NodeStructs::CompileTimeType>(std::get<non_type_information>(right.value()).type)) });
			return "";
		}
		if (holds<NodeStructs::PrimitiveType>(std::get<non_type_information>(right.value()).type)) {
			if (variables.at(var).size() != 1)
				throw;
			variables.at(var).pop_back();
			variables.at(var).push_back(variable_info{
				.value_category = NodeStructs::Value{},
				.type = NodeStructs::CompileTimeType{ std::move(get<NodeStructs::PrimitiveType>(std::get<non_type_information>(right.value()).type)) }
			} );
			return "";
		}
		throw;
	}
	else {
		auto left = transpile_expression(state, variables, statement.left);
		return_if_error(left);
		auto right = transpile_expression(state, variables, statement.right);
		return_if_error(right);
		if (!std::holds_alternative<non_type_information>(left.value()))
			throw;
		if (!std::holds_alternative<non_type_information>(right.value()))
			throw;
		return std::get<non_type_information>(left.value()).representation + " = " + std::get<non_type_information>(right.value()).representation + ";\n";
	}
}

R transpile_statement(
	transpilation_state_with_indent state,
	variables_t& variables,
	const NodeStructs::MetaType& expected_return_type,
	const NodeStructs::Statement<function_context>& statement
) {
	return std::visit(overload(
		[&](const NodeStructs::CompileTimeStatement<function_context>& st) -> R {
			return caesium_lib::variant::visit(st, [&](const auto& st) -> R {
				return transpile_statement_specific<function_context, true>(state, variables, expected_return_type, st);
			});
		},
		[&](const Variant<NodeStructs::RunTimeStatement>& st) -> R {
			return caesium_lib::variant::visit(std::get<NodeStructs::RunTimeStatement>(st._value),
				[&](const auto& st) -> R {
					return transpile_statement_specific<function_context, false>(state, variables, expected_return_type, st);
				}
			);
		}
	), statement.statement.get()._value);
	/*if (statement.is_compile_time)
		return std::visit(
			[&](const auto& st) -> R {
				return transpile_statement_specific<function_context, true>(state, variables, expected_return_type, st);
			},
			statement.statement.get()._value
		);
	else
		return std::visit(
			[&](const auto& st) -> R {
				return transpile_statement_specific<function_context, false>(state, variables, expected_return_type, st);
			},
			statement.statement.get()._value
		);*/
}

R transpile_statement(
	transpilation_state_with_indent state,
	variables_t& variables,
	const NodeStructs::MetaType& expected_return_type,
	const NodeStructs::Statement<grammar::type_context>& statement
) {
	throw;
	//return transpile_statement_visitor<context>{ {}, state, variables, expected_return_type }(statement);
}

R transpile_statement(
	transpilation_state_with_indent state,
	variables_t& variables,
	const NodeStructs::MetaType& expected_return_type,
	const NodeStructs::Statement<grammar::top_level_context>& statement
) {
	throw;
	//return transpile_statement_visitor<context>{ {}, state, variables, expected_return_type }(statement);
}
