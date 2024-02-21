#pragma once
#include "../core/toCPP.hpp"
#include "statement_visitor.hpp"

struct transpile_statement_visitor : StatementVisitor<transpile_statement_visitor> {
	using StatementVisitor<transpile_statement_visitor>::operator();

	transpilation_state_with_indent state;

	using R = transpile_t;

	R operator()(const NodeStructs::Expression& statement);
	R operator()(const NodeStructs::VariableDeclarationStatement& statement);
	R operator()(const NodeStructs::IfStatement& statement);
	R operator()(const NodeStructs::ForStatement& statement);
	R operator()(const NodeStructs::IForStatement& statement);
	R operator()(const NodeStructs::WhileStatement& statement);
	R operator()(const NodeStructs::BreakStatement& statement);
	R operator()(const NodeStructs::ReturnStatement& statement);
	R operator()(const NodeStructs::BlockStatement& statement);
};