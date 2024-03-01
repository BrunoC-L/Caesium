#pragma once
#include "../core/toCPP.hpp"
#include "type_category_visitor.hpp"

struct typename_of_type_visitor : TypeCategoryVisitor<typename_of_type_visitor> {
	using TypeCategoryVisitor<typename_of_type_visitor>::operator();

	transpilation_state_with_indent state;

	using R = expected<NodeStructs::Typename>;

	TypeCategoryVisitorDeclarations
};

expected<NodeStructs::Typename> typename_of_type(
	transpilation_state_with_indent state,
	const auto& type
) {
	return typename_of_type_visitor{ {}, state }(type);
}
