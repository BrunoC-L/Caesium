#pragma once
#include "../core/toCPP.hpp"
#include "typename_visitor.hpp"

struct typename_original_representation_visitor : TypenameVisitor<typename_original_representation_visitor> {
	using TypenameVisitor<typename_original_representation_visitor>::operator();

	using R = std::string;

	TypenameVisitorDeclarations
};

std::string typename_original_representation(
	const auto& tn
) {
	return typename_original_representation_visitor{ {} }(tn);
}
