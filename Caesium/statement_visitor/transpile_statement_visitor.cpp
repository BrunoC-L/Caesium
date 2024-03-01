#include "../core/toCPP.hpp"

using T = transpile_statement_visitor;
using R = T::R;

R T::operator()(const NodeStructs::Expression& statement) {
	return transpile_expression(state, statement).transform([](auto s) { return std::move(s).representation + ";\n"; });
}

R T::operator()(const NodeStructs::VariableDeclarationStatement& statement) {
	bool is_aggregate = std::holds_alternative<NodeStructs::BraceArguments>(statement.expr.expression.get());
	if (statement.type <=> NodeStructs::Typename{ NodeStructs::BaseTypename{ "auto" } } == std::weak_ordering::equivalent) {

		if (is_aggregate)
			return error{
				"user error",
				"cannot use aggregate initialization for auto variables"
			};

		auto assigned_expression = transpile_expression(state, statement.expr);
		return_if_error(assigned_expression);

		auto deduced_typename = typename_of_type(state, assigned_expression.value().type);
		return_if_error(deduced_typename);

		auto deduced_typename_repr = transpile_typename(state, deduced_typename.value());
		return_if_error(deduced_typename);

		state.state.variables[statement.name].push_back({ NodeStructs::MutableReference{}, assigned_expression.value().type });

		return deduced_typename_repr.value() + " " + statement.name + " = " + assigned_expression.value().representation + ";\n";
	}
	else if (is_aggregate) {
		auto type = type_of_typename(state, statement.type);
		return_if_error(type);

		auto type_repr = transpile_typename(state, statement.type);
		return_if_error(type_repr);

		const auto& aggregate = std::get<NodeStructs::BraceArguments>(statement.expr.expression.get());

		auto as_construct = NodeStructs::ConstructExpression{
			.operand = statement.type,
			.arguments = aggregate.args
		};

		auto assigned_expression = transpile_expression(state, as_construct);
		return_if_error(assigned_expression);

		state.state.variables[statement.name].push_back({ NodeStructs::MutableReference{}, type.value() });

		return type_repr.value() + " " + statement.name + " = " + assigned_expression.value().representation + ";\n";
	}
	else {
		auto assigned_expression = transpile_expression(state, statement.expr);
		return_if_error(assigned_expression);

		auto type = type_of_typename(state, statement.type);
		return_if_error(type);

		auto type_repr = transpile_typename(state, statement.type);
		return_if_error(type_repr);

		state.state.variables[statement.name].push_back({ NodeStructs::MutableReference{}, type.value() });

		return type_repr.value() + " " + statement.name + " = " + assigned_expression.value().representation + ";\n";
	}
}

R T::operator()(const NodeStructs::IfStatement& statement) {
	auto if_statements = transpile(state.indented(), statement.ifStatements);
	return_if_error(if_statements);
	if (statement.elseExprStatements.has_value())
		return "if (" +
		transpile_expression(state, statement.ifExpr).value().representation +
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
	else
		return "if (" +
		transpile_expression(state, statement.ifExpr).value().representation +
			") {\n" +
			if_statements.value() +
			indent(state.indent) +
			"}\n";
}

R T::operator()(const NodeStructs::ForStatement& statement) {
	auto coll_type_or_e = transpile_expression(state, statement.collection);
	return_if_error(coll_type_or_e);
	auto it_type = iterator_type(state, coll_type_or_e.value().type);

	std::stringstream ss;
	if (statement.iterators.size() > 1) {
		bool can_be_decomposed = [&]() {
			if (std::holds_alternative<std::reference_wrapper<const NodeStructs::Type>>(it_type.value)) {
				const auto t = std::get<std::reference_wrapper<const NodeStructs::Type>>(it_type.value);
				if (t.get().name == "Int")
					return false;
			}
			return true;
		}();
		if (!can_be_decomposed)
			return error{ "user error", "" };
		auto opt_e = add_decomposed_for_iterator_variables(state, statement.iterators, it_type);
		if (opt_e.has_value())
			return opt_e.value();
		ss << "for (auto&& [";
		bool first = true;
		for (const auto& iterator : statement.iterators) {
			if (first)
				first = false;
			else
				ss << ", ";
			ss << std::visit(overload(overload_default_error,
				[&](const NodeStructs::VariableDeclaration& iterator) {
					return iterator.name;
				},
				[&](const std::string& iterator) {
					return iterator;
				}
			), iterator);
		}
		ss << "]";
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
	auto s2 = transpile(state.indented(), statement.statements);
	return_if_error(s2);
	ss << " : "
		<< s1.value().representation
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
	return "while (" + transpile_expression(state, statement.whileExpr).value().representation + ") {\n" + transpile(state, statement.statements).value() + "}";
}

R T::operator()(const NodeStructs::BreakStatement& statement) {
	if (statement.ifExpr.has_value())
		return "if (" + transpile_expression(state, statement.ifExpr.value()).value().representation + ") break;\n";
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
		return "if (" + cnd.value().representation + ") return " + return_expression.value() + ";\n";
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
