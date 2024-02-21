#pragma once
#include "../core/node_structs.hpp"

template <typename T>
concept StatementVisitorConcept = requires(T && t, const NodeStructs::Statement & v) {
	t(v);
	std::visit(t, v);
};

template <typename T>
struct StatementVisitor {
	template <typename Self>
	auto operator()(this Self&& self, const NodeStructs::Statement& t) {
		return std::visit(
			[&](const auto& t) {
				return self(t);
			},
			t.statement
		);
	}
	/*
	R operator()(const NodeStructs::Expression& statement);
	R operator()(const NodeStructs::VariableDeclarationStatement& statement);
	R operator()(const NodeStructs::IfStatement& statement);
	R operator()(const NodeStructs::ForStatement& statement);
	R operator()(const NodeStructs::IForStatement& statement);
	R operator()(const NodeStructs::WhileStatement& statement);
	R operator()(const NodeStructs::BreakStatement& statement);
	R operator()(const NodeStructs::ReturnStatement& statement);
	R operator()(const NodeStructs::BlockStatement& statement);
	*/
};
