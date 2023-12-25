#include "transpile_statement_visitor.hpp"
#include "type_of_typename_visitor.hpp"
#include "transpile_expression_visitor.hpp"
#include "type_of_expression_visitor.hpp"
#include "transpile_typename_visitor.hpp"

using T = transpile_statement_visitor;
using R = T::R;

R T::operator()(const NodeStructs::Expression& statement) {
	return transpile_expression_visitor{ {}, variables, named }(statement).transform([](auto s) { return std::move(s) + ";\n"; });
}
R T::operator()(const NodeStructs::VariableDeclarationStatement& statement) {
	variables[statement.name].push_back({ NodeStructs::Value{}, type_of_typename_visitor{ {}, variables, named }(statement.type) });
	return transpile_typename_visitor{ {}, variables, named }(statement.type).value() + " " + statement.name + " = " + transpile_expression_visitor{{}, variables, named}(statement.expr).value() + ";\n";
}
R T::operator()(const NodeStructs::IfStatement& statement) {
	if (statement.elseExprStatements.has_value())
		return "if (" +
		transpile_expression_visitor{ {}, variables, named }(statement.ifExpr).value() +
		") {\n" +
		transpile(variables, named, statement.ifStatements).value() +
		"} else " +
		std::visit(
			overload(overload_default_error,
				[&](const Box<NodeStructs::IfStatement>& elseif) {
					return operator()(elseif.get()).value();
				},
				[&](const std::vector<NodeStructs::Statement>& justelse) {
					return "{" + transpile(variables, named, justelse).value() + "}";
				}
			),
			statement.elseExprStatements.value()
		);
	else
		return "if (" +
		transpile_expression_visitor{ {}, variables, named }(statement.ifExpr).value() +
		") {\n" +
		transpile(variables, named, statement.ifStatements).value() +
		"}";
}
R T::operator()(const NodeStructs::ForStatement& statement) {
	auto coll_type_or_e = type_of_expression_visitor{ {}, variables, named }(statement.collection);
	if (!coll_type_or_e.has_value())
		return std::unexpected{ std::move(coll_type_or_e).error() };
	auto it_type = iterator_type(variables, named, coll_type_or_e.value().second);
	
	bool can_be_decomposed = [&]() {
		if (std::holds_alternative<std::reference_wrapper<const NodeStructs::Type>>(it_type.value)) {
			const auto t = std::get<std::reference_wrapper<const NodeStructs::Type>>(it_type.value);
			if (t.get().name == "Int")
				return false;
		}
		return true;
		}();
		add_for_iterator_variables(variables, named, statement, it_type);
	
		std::stringstream ss;
		if (can_be_decomposed) {
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
			if (statement.iterators.size() > 1)
				throw std::runtime_error("Expected 1 iterator");
			ss << "for (auto&& " << std::visit(overload(overload_default_error,
				[&](const NodeStructs::VariableDeclaration& iterator) {
					return iterator.name;
				},
				[&](const std::string& iterator) {
					return iterator;
				}
			), statement.iterators.at(0));
		}
		auto s1 = transpile_expression_visitor{ {}, variables, named }(statement.collection);
		auto s2 = transpile(variables, named, statement.statements);
		if (!s1.has_value())
			return std::unexpected{ s1.error() };
		if (!s2.has_value())
			return std::unexpected{ s2.error() };
		ss << " : "
			<< s1.value()
			<< ") {\n"
			<< s2.value()
			<< "}\n";
	
		remove_for_iterator_variables(variables, statement);
		return ss.str();
}
R T::operator()(const NodeStructs::IForStatement& statement) {
	throw;
}
R T::operator()(const NodeStructs::WhileStatement& statement) {
	return "while (" + transpile_expression_visitor{ {}, variables, named }(statement.whileExpr).value() + ") {\n" + transpile(variables, named, statement.statements).value() + "}";
}
R T::operator()(const NodeStructs::BreakStatement& statement) {
	if (statement.ifExpr.has_value())
		return "if (" + transpile_expression_visitor{ {}, variables, named }(statement.ifExpr.value()).value() + ") break;\n";
	else
		return "break;\n";
}
R T::operator()(const NodeStructs::ReturnStatement& statement) {
	std::string return_value = [&]() {
		if (statement.returnExpr.size() == 0)
			return std::string{};
		else if (statement.returnExpr.size() == 1)
			return transpile_expression_visitor{ {}, variables, named }(statement.returnExpr.at(0)).value();
		else {
			throw;
			//return "{" + transpile_args(variables, named, statement.returnExpr).value() + "}";
		}
		}();
		if (statement.ifExpr.has_value())
			return "if (" + transpile_expression_visitor{ {}, variables, named }(statement.ifExpr.value()).value() + ") return " + return_value + ";\n";
		else
			return "return " + return_value + ";\n";
}
R T::operator()(const NodeStructs::BlockStatement& statement) {
	auto s = std::get<NodeStructs::BaseTypename>(statement.parametrized_block.value).type;
	if (named.blocks.contains(s)) {
		const NodeStructs::Block& block = *named.blocks.at(s);
		std::stringstream ss;
		for (const auto& statement_in_block : block.statements)
			ss << operator()(statement_in_block).value();
		return ss.str();
	}
	else {
		throw std::runtime_error("bad block name" + s);
	}
}
