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
			t.statement._value
		);
	}
};

#define StatementExpands_(Expand) \
	Expand(NodeStructs::Expression)\
	Expand(NodeStructs::VariableDeclarationStatement)\
	Expand(NodeStructs::IfStatement)\
	Expand(NodeStructs::IForStatement)\
	Expand(NodeStructs::ForStatement)\
	Expand(NodeStructs::WhileStatement)\
	Expand(NodeStructs::BreakStatement)\
	Expand(NodeStructs::ReturnStatement)\
	Expand(NodeStructs::BlockStatement)\
	Expand(NodeStructs::MatchStatement)\
	Expand(NodeStructs::SwitchStatement)\
	Expand(NodeStructs::EqualStatement)

#define statement_decl(T) R operator()(const T&);
#define StatementVisitorDeclarations StatementExpands_(statement_decl)
