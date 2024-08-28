#pragma once
#include "../core/toCPP.hpp"
#include "typename_visitor.hpp"

struct typename_for_template_visitor : TypenameVisitor<typename_for_template_visitor> {
	using TypenameVisitor<typename_for_template_visitor>::operator();

	using R = std::string;

	TypenameVisitorDeclarations
};

std::string typename_for_template(
	const auto& tn
) {
	return typename_for_template_visitor{ {} }(tn);
}
