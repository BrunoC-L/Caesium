#include "transpile_statement_visitor.hpp"
#include "type_of_typename_visitor.hpp"
#include "transpile_expression_visitor.hpp"
#include "type_of_expression_visitor.hpp"
#include "transpile_typename_visitor.hpp"

using T = transpile_statement_visitor;
using R = T::R;

R T::operator()(const NodeStructs::Expression& statement) {
	return transpile_expression_visitor{ {}, state }(statement).transform([](auto s) { return std::move(s) + ";\n"; });
}

R T::operator()(const NodeStructs::VariableDeclarationStatement& statement) {
	state.state.variables[statement.name].push_back({ NodeStructs::Value{}, type_of_typename_visitor{ {}, state }(statement.type).value() });
	auto type = transpile_typename_visitor{ {}, state }(statement.type);
	if (!type.has_value())
		return std::unexpected{ type.error() };
	auto variable_name = transpile_expression_visitor{ {}, state }(statement.expr);
	if (!variable_name.has_value())
		return std::unexpected{ variable_name.error() };
	return type.value() + " " + statement.name + " = " + variable_name.value() + ";\n";
}

R T::operator()(const NodeStructs::IfStatement& statement) {
	auto if_statements = transpile(state.indented(), statement.ifStatements);
	if (!if_statements.has_value())
		return std::unexpected{ if_statements.error() };
	if (statement.elseExprStatements.has_value())
		return "if (" +
		transpile_expression_visitor{ {}, state }(statement.ifExpr).value() +
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
			transpile_expression_visitor{ {}, state }(statement.ifExpr).value() +
			") {\n" +
			if_statements.value() +
			indent(state.indent) +
			"}\n";
}

R T::operator()(const NodeStructs::ForStatement& statement) {
	auto coll_type_or_e = type_of_expression_visitor{ {}, state }(statement.collection);
	if (!coll_type_or_e.has_value())
		return std::unexpected{ std::move(coll_type_or_e).error() };
	auto it_type = iterator_type(state, coll_type_or_e.value().second);

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
			return std::unexpected{ user_error{ "" } };
		auto opt_e = add_decomposed_for_iterator_variables(state, statement.iterators, it_type);
		if (opt_e.has_value())
			return std::unexpected{ opt_e.value() };
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
			return std::unexpected{ opt_e.value() };
		ss << "for (auto&& " << std::visit(overload(overload_default_error,
			[&](const NodeStructs::VariableDeclaration& iterator) {
				return iterator.name;
			},
			[&](const std::string& iterator) {
				return iterator;
			}
		), statement.iterators.at(0));
	}
	
	auto s1 = transpile_expression_visitor{ {}, state }(statement.collection);
	auto s2 = transpile(state.indented(), statement.statements);
	if (!s1.has_value())
		return std::unexpected{ s1.error() };
	if (!s2.has_value())
		return std::unexpected{ s2.error() };
	ss << " : "
		<< s1.value()
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
	return "while (" + transpile_expression_visitor{ {}, state }(statement.whileExpr).value() + ") {\n" + transpile(state, statement.statements).value() + "}";
}

R T::operator()(const NodeStructs::BreakStatement& statement) {
	if (statement.ifExpr.has_value())
		return "if (" + transpile_expression_visitor{ {}, state }(statement.ifExpr.value()).value() + ") break;\n";
	else
		return "break;\n";
}

R T::operator()(const NodeStructs::ReturnStatement& statement) {
	if (statement.returnExpr.size() == 0)
		return "return;\n";
	R return_expression = [&]() -> R {
		if (statement.returnExpr.size() == 1)
			return transpile_args(state, statement.returnExpr).value();
		else
			return "{" + transpile_args(state, statement.returnExpr).value() + "}";
	}();
	if (!return_expression.has_value())
		return std::unexpected{ return_expression.error() };
	if (statement.ifExpr.has_value()) {
		auto cnd = transpile_expression_visitor{ {}, state }(statement.ifExpr.value());
		if (!cnd.has_value())
			return std::unexpected{ cnd.error() };
		return "if (" + cnd.value() + ") return " + return_expression.value() + ";\n";
	}
	else
		return "return " + return_expression.value() + ";\n";
}

R T::operator()(const NodeStructs::BlockStatement& statement) {
	auto s = std::get<NodeStructs::BaseTypename>(statement.parametrized_block.value).type;
	if (state.state.named.blocks.contains(s)) {
		const NodeStructs::Block& block = *state.state.named.blocks.at(s);
		std::stringstream ss;
		for (const auto& statement_in_block : block.statements)
			ss << operator()(statement_in_block).value();
		return ss.str();
	}
	else {
		throw std::runtime_error("bad block name" + s);
	}
}
