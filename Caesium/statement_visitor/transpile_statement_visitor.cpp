#include "../core/toCPP.hpp"

using T = transpile_statement_visitor;
using R = T::R;

R T::operator()(const NodeStructs::Expression& statement) {
	auto repr =  transpile_expression(state, variables, statement);
	return_if_error(repr);
	if (!std::holds_alternative<non_type_information>(repr.value()))
		throw;
	const auto& repr_ok = std::get<non_type_information>(repr.value());
	return repr_ok.representation + ";\n";
}

R T::operator()(const NodeStructs::VariableDeclarationStatement& statement) {
	bool is_aggregate_init = std::holds_alternative<NodeStructs::BraceArguments>(statement.expr.expression.get()._value);
	bool is_auto = statement.type <=> NodeStructs::Typename{ NodeStructs::BaseTypename{ "auto" } } == std::weak_ordering::equivalent;
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

			auto deduced_typename = typename_of_type(state, assigned_expression_ok.type.type);
			return_if_error(deduced_typename);

			auto deduced_typename_repr = transpile_typename(state, deduced_typename.value());
			return_if_error(deduced_typename_repr);

			variables[statement.name].push_back({ NodeStructs::MutableReference{}, std::move(assigned_expression_ok).type.type });

			return deduced_typename_repr.value() + " " + statement.name + " = " + assigned_expression_ok.representation + ";\n";
		}
		throw;
	}
	else {
		auto type = type_of_typename(state, statement.type);
		return_if_error(type);

		auto type_repr = transpile_typename(state, statement.type);
		return_if_error(type_repr);

		auto assigned_expression = [&](){
			if (is_aggregate_init) {
				const auto& aggregate = std::get<NodeStructs::BraceArguments>(statement.expr.expression.get()._value);
				auto as_construct = NodeStructs::ConstructExpression{
					.operand = copy(statement.type),
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

		const auto& assigned_expression_ok = std::get<non_type_information>(assigned_expression.value());

		if (!is_assignable_to(state, type.value(), assigned_expression_ok.type.type))
			return error{
				"user error",
				"not assignable"
			};

		variables[statement.name].push_back({ NodeStructs::MutableReference{}, copy(type.value()) });

		if (std::holds_alternative<NodeStructs::UnionType>(assigned_expression_ok.type.type.type._value)
			&& (type.value() <=> assigned_expression_ok.type.type != std::weak_ordering::equivalent)) {
			std::string lambda_var_name = [&]() { std::stringstream ss; ss << "auto" << state.state.current_variable_unique_id++; return ss.str(); }();
			return type_repr.value() + " " + statement.name + " = "
				"std::visit([](const auto& " + lambda_var_name + ") -> " + type_repr.value() + " {"
					" return " + lambda_var_name + "; "
				"}, " + assigned_expression_ok.representation + ");\n";
		}
		else {
			if (assigned_expression_ok.type.type <=> type.value() == std::weak_ordering::equivalent)
				return type_repr.value() + " " + statement.name + " = " + assigned_expression_ok.representation + ";\n";
			else
				return type_repr.value() + " " + statement.name + " = { " + assigned_expression_ok.representation + " };\n";
		}
	}
}

R T::operator()(const NodeStructs::IfStatement& statement) {
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
		"} else " +
		std::visit(
			overload(overload_default_error,
				[&](const NonCopyableBox<NodeStructs::IfStatement>& elseif) {
					return operator()(elseif.get()).value();
				},
				[&](const std::vector<NodeStructs::Statement>& justelse) {
					return "{" + transpile(state, variables, justelse, expected_return_type).value() + "}";
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

R T::operator()(const NodeStructs::ForStatement& statement) {
	auto coll_type_or_e = transpile_expression(state, variables, statement.collection);
	return_if_error(coll_type_or_e);
	if (!std::holds_alternative<non_type_information>(coll_type_or_e.value()))
		throw;
	const auto& coll_type_or_e_ok = std::get<non_type_information>(coll_type_or_e.value());
	auto it_type = iterator_type(state, coll_type_or_e_ok.type.type);

	std::stringstream ss;
	if (statement.iterators.size() > 1) {
		throw;
	}
	else {
		auto opt_e = add_for_iterator_variable(state, variables, statement.iterators, it_type);
		if (opt_e.has_value())
			return opt_e.value();
		ss << "for (auto&& " << std::visit(overload(overload_default_error,
			[&](const NodeStructs::VariableDeclaration& iterator) {
				return iterator.name;
			},
			[&](const std::string& iterator) {
				return iterator;
			}
		), statement.iterators.at(0)._value);
	}
	
	auto s1 = transpile_expression(state, variables, statement.collection);
	return_if_error(s1);
	if (!std::holds_alternative<non_type_information>(s1.value()))
		throw;
	const auto& s1_ok = std::get<non_type_information>(s1.value());
	auto s2 = transpile(state.indented(), variables, statement.statements, expected_return_type);
	return_if_error(s2);
	ss << " : "
		<< s1_ok.representation
		<< ") {\n"
		<< s2.value()
		<< indent(state.indent)
		<< "}\n";
	return ss.str();
}

R T::operator()(const NodeStructs::IForStatement& statement) {
	throw;
}

R T::operator()(const NodeStructs::WhileStatement& statement) {
	auto s1 = transpile_expression(state, variables, statement.whileExpr);
	return_if_error(s1);
	if (!std::holds_alternative<non_type_information>(s1.value()))
		throw;
	const auto& s1_ok = std::get<non_type_information>(s1.value());
	auto s2 = transpile(state.indented(), variables, statement.statements, expected_return_type);
	return_if_error(s2);
	return "while (" + s1_ok.representation + ") {\n" + s2.value() + "}";
}

R T::operator()(const NodeStructs::BreakStatement& statement) {
	if (!statement.ifExpr.has_value())
		return "break;\n";
	auto s1 = transpile_expression(state, variables, statement.ifExpr.value());
	return_if_error(s1);
	if (!std::holds_alternative<non_type_information>(s1.value()))
		throw;
	const auto& s1_ok = std::get<non_type_information>(s1.value());
	return "if (" + s1_ok.representation + ") break;\n";
}

R T::operator()(const NodeStructs::ReturnStatement& statement) {
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
		if (!is_assignable_to(state, expected_return_type, expr_info_ok.type.type)) {
			is_assignable_to(state, expected_return_type, expr_info_ok.type.type);
			throw;
		}
		return expr_info_ok.representation;
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

R T::operator()(const NodeStructs::BlockStatement& statement) {
	auto s = std::get<NodeStructs::BaseTypename>(statement.parametrized_block.value._value).type;
	if (state.state.global_namespace.blocks.contains(s)) {
		const NodeStructs::Block& block = state.state.global_namespace.blocks.at(s).back();
		std::stringstream ss;
		for (const auto& statement_in_block : block.statements)
			ss << operator()(statement_in_block).value();
		return ss.str();
	}
	else {
		throw std::runtime_error("bad block name" + s);
	}
}

R T::operator()(const NodeStructs::MatchStatement& statement) {
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
	auto tn = typename_of_type(state, expr_ok.type.type);
	return_if_error(tn);
	auto tn_repr = transpile_typename(state, tn.value());
	return_if_error(tn_repr);
	unsigned id = state.state.current_variable_unique_id++;
	ss << "const " << tn_repr.value() << "& matchval" << id << " = " << expr_ok.representation << ";\n";
	for (const NodeStructs::MatchCase& match_case : statement.cases) {
		auto tn = transpile_typename(state, match_case.variable_declarations.at(0).first);
		return_if_error(tn);
		auto varname = match_case.variable_declarations.at(0).second;

		variables[varname]
			.push_back(variable_info{
				.value_category = NodeStructs::Reference{},
				.type = type_of_typename(state, match_case.variable_declarations.at(0).first).value() 
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
	// here we would actually know at compile time that this cant be hit so we wouldnt actually insert a throw if the code was complete
	ss << indent(state.indent + 1) << "throw;\n";
	return ss.str();
}

R T::operator()(const NodeStructs::SwitchStatement& statement) {
	throw;
}

R T::operator()(const NodeStructs::EqualStatement& statement) {
	auto left = transpile_expression(state, variables, statement.left);
	auto right = transpile_expression(state, variables, statement.right);
	return_if_error(left);
	return_if_error(right);
	if (!std::holds_alternative<non_type_information>(left.value()))
		throw;
	if (!std::holds_alternative<non_type_information>(right.value()))
		throw;
	return std::get<non_type_information>(left.value()).representation + " = " + std::get<non_type_information>(right.value()).representation;
}
