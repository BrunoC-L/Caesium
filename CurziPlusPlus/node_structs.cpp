#include "node_structs.h"

template <typename T>
std::weak_ordering cmp(const T& a, const T& b) {
	if constexpr (is_specialization<T, std::vector>::value) {
		if (auto size_cmp = cmp(a.size(), b.size()); size_cmp != 0)
			return size_cmp;

		for (size_t i = 0; i < a.size(); ++i)
			if (auto v = cmp(a.at(i), b.at(i)); v != 0)
				return v;

		return std::weak_ordering::equivalent;
	}
	else if constexpr (is_specialization<T, std::optional>::value) {
		return a.has_value() && b.has_value() ? cmp(a.value(), b.value()) : cmp(a.has_value(), b.has_value());
	}
	else if constexpr (is_specialization<T, std::variant>::value) {
		auto index_cmp = cmp(a.index(), b.index());
		if (index_cmp != 0)
			return index_cmp;
		return std::visit(
			[&](const auto& _a) {
				return cmp(_a, std::get<std::remove_cvref_t<decltype(_a)>>(b));
			},
			a
		);
	}
	else if constexpr (is_specialization<T, std::reference_wrapper>::value) {
		return cmp(a.get(), b.get());
	}
	else
		return a <=> b;
}

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

	std::weak_ordering Typename::operator<=>(const Typename& other) const {
		return cmp(value, other.value);
	}

	std::weak_ordering TypeVariant::operator<=>(const TypeVariant& other) const {
		return cmp(value, other.value);
	}

	std::weak_ordering TypeTemplateInstance::operator<=>(const TypeTemplateInstance& other) const {
		if (auto c = cmp(type_template.get(), other.type_template.get()); c != 0)
			return c;
		return cmp(template_arguments, other.template_arguments);
	}
}
