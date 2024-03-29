#include "../core/toCPP.hpp"

using T = transpile_statement_visitor;
using R = T::R;

R T::operator()(const NodeStructs::Expression& statement) {
	auto repr =  transpile_expression(state, statement);
	return_if_error(repr);
	if (!std::holds_alternative<non_type_information>(repr.value()))
		throw;
	const auto& repr_ok = std::get<non_type_information>(repr.value());
	return repr_ok.representation + ";\n";
}

R T::operator()(const NodeStructs::VariableDeclarationStatement& statement) {
	bool is_aggregate_init = std::holds_alternative<NodeStructs::BraceArguments>(statement.expr.expression.get());
	bool is_auto = statement.type <=> NodeStructs::Typename{ NodeStructs::BaseTypename{ "auto" } } == std::weak_ordering::equivalent;
	if (is_auto) {
		if (is_aggregate_init)
			return error{
				"user error",
				"auto cannot deduce aggregate initialization"
			};

		auto assigned_expression = transpile_expression(state, statement.expr);
		return_if_error(assigned_expression);
		if (std::holds_alternative<non_type_information>(assigned_expression.value())) {
			const auto& assigned_expression_ok = std::get<non_type_information>(assigned_expression.value());

			auto deduced_typename = typename_of_type(state, assigned_expression_ok.type.type);
			return_if_error(deduced_typename);

			auto deduced_typename_repr = transpile_typename(state, deduced_typename.value());
			return_if_error(deduced_typename_repr);

			state.state.variables[statement.name].push_back({ NodeStructs::MutableReference{}, assigned_expression_ok.type.type });

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
				const auto& aggregate = std::get<NodeStructs::BraceArguments>(statement.expr.expression.get());
				auto as_construct = NodeStructs::ConstructExpression{
					.operand = statement.type,
					.arguments = aggregate.args
				};
				return transpile_expression(state, as_construct);
			}
			else
				return transpile_expression(state, statement.expr);
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

		state.state.variables[statement.name].push_back({ NodeStructs::MutableReference{}, type.value() });

		if (std::holds_alternative<NodeStructs::UnionType>(assigned_expression_ok.type.type.type)
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
	auto if_statements = transpile(state.indented(), statement.ifStatements);
	return_if_error(if_statements);

	auto if_expr = transpile_expression(state, statement.ifExpr);
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
				[&](const Box<NodeStructs::IfStatement>& elseif) {
					return operator()(elseif.get()).value();
				},
				[&](const std::vector<NodeStructs::Statement>& justelse) {
					return "{" + transpile(state, justelse).value() + "}";
				}
			),
			statement.elseExprStatements.value()
		);
	else {
		auto k = transpile_expression(state, statement.ifExpr);
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
	auto coll_type_or_e = transpile_expression(state, statement.collection);
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
		auto opt_e = add_for_iterator_variable(state, statement.iterators, it_type);
		if (opt_e.has_value())
			return opt_e.value();
		ss << "for (auto&& " << std::visit(overload(overload_default_error,
			[&](const NodeStructs::VariableDeclaration& iterator) {
				return iterator.name;
			},
			[&](const std::string& iterator) {
				return iterator;
			}
		), statement.iterators.at(0));
	}
	
	auto s1 = transpile_expression(state, statement.collection);
	return_if_error(s1);
	if (!std::holds_alternative<non_type_information>(s1.value()))
		throw;
	const auto& s1_ok = std::get<non_type_information>(s1.value());
	auto s2 = transpile(state.indented(), statement.statements);
	return_if_error(s2);
	ss << " : "
		<< s1_ok.representation
		<< ") {\n"
		<< s2.value()
		<< indent(state.indent)
		<< "}\n";
	
	remove_for_iterator_variables(state, statement);
	return ss.str();
}

R T::operator()(const NodeStructs::IForStatement& statement) {
	throw;
}

R T::operator()(const NodeStructs::WhileStatement& statement) {
	auto s1 = transpile_expression(state, statement.whileExpr);
	return_if_error(s1);
	if (!std::holds_alternative<non_type_information>(s1.value()))
		throw;
	const auto& s1_ok = std::get<non_type_information>(s1.value());
	auto s2 = transpile(state.indented(), statement.statements);
	return_if_error(s2);
	return "while (" + s1_ok.representation + ") {\n" + s2.value() + "}";
}

R T::operator()(const NodeStructs::BreakStatement& statement) {
	auto s1 = transpile_expression(state, statement.ifExpr.value());
	return_if_error(s1);
	if (!std::holds_alternative<non_type_information>(s1.value()))
		throw;
	const auto& s1_ok = std::get<non_type_information>(s1.value());
	if (statement.ifExpr.has_value())
		return "if (" + s1_ok.representation + ") break;\n";
	else
		return "break;\n";
}

R T::operator()(const NodeStructs::ReturnStatement& statement) {
	if (statement.returnExpr.size() == 0)
		return "return;\n";
	R return_expression = [&]() -> R {
		auto args_repr = transpile_args(state, statement.returnExpr);
		return_if_error(args_repr);
		if (statement.returnExpr.size() == 1)
			return args_repr.value();
		else
			return "{" + args_repr.value() + "}";
	}();
	return_if_error(return_expression);
	if (statement.ifExpr.has_value()) {
		auto cnd = transpile_expression(state, statement.ifExpr.value());
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
	auto s = std::get<NodeStructs::BaseTypename>(statement.parametrized_block.value).type;
	if (state.state.named.blocks.contains(s)) {
		const NodeStructs::Block& block = *state.state.named.blocks.at(s).back();
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
	auto expr_info = transpile_expression(state, statement.expressions.at(0));
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

		state.state.variables[varname]
			.push_back(variable_info{
				.value_category = NodeStructs::Reference{},
				.type = type_of_typename(state, match_case.variable_declarations.at(0).first).value() 
			});
		auto statements = transpile(state.indented(), match_case.statements);
		state.state.variables[varname].pop_back();
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
