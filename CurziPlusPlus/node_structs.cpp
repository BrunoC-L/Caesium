#include "node_structs.h"
namespace NodeStructs {
	std::weak_ordering Expression::operator<=>(const Expression& other) const {
		return cmp(expression.get(), other.expression.get());
	}

	std::weak_ordering IfStatement::operator<=>(const IfStatement& other) const {
		if (auto c = cmp(ifStatements, other.ifStatements); c != 0)
			return c;
		if (auto c = cmp(ifExpr, other.ifExpr); c != 0)
			return c;
		return cmp(elseExprStatements, other.elseExprStatements);
	}

	std::weak_ordering Statement::operator<=>(const Statement& other) const {
		return cmp(statement, other.statement);
	}

	std::weak_ordering BreakStatement::operator<=>(const BreakStatement& other) const {
		return cmp(ifExpr, other.ifExpr);
	}

	std::weak_ordering ReturnStatement::operator<=>(const ReturnStatement& other) const {
		if (auto c = cmp(ifExpr, other.ifExpr); c != 0)
			return c;
		return cmp(returnExpr, other.returnExpr);
	}

	std::weak_ordering UnaryExpression::operator<=>(const UnaryExpression& other) const {
		if (auto c = cmp(expr, other.expr); c != 0)
			return c;
		return cmp(unary_operator, other.unary_operator);
	}
}
