#include "node_structs.h"

namespace NodeStructs {
	//TemplatedTypename::TemplatedTypename(const TemplatedTypename& other) : 
	//	type(std::make_unique<NodeStructs::Typename>(*other.type.get())), 
	//	templated_with(other.templated_with) {}

	///*TemplatedTypename& TemplatedTypename::operator=(const TemplatedTypename& other) {
	//	this->type = std::make_unique<NodeStructs::Typename>(*other.type.get());
	//	this->templated_with = other.templated_with;
	//}*/

	//TemplatedTypename::TemplatedTypename(std::unique_ptr<Typename>&& type, std::vector<Typename>&& templated_with) :
	//type(std::move(type)), templated_with(std::move(templated_with)) {}

	bool TemplatedTypename::operator==(const TemplatedTypename& other) const {
		return *type == *other.type && templated_with == other.templated_with;
	}

	bool TemplatedTypename::operator==(const Typename& other) const {
		return std::visit(
			overload(
				[&](const TemplatedTypename& t) {
					return (*this) == t;
				},
				[](const auto&) {
					return false;
				}
			),
			other
		);
	}

	//NamespacedTypename::NamespacedTypename(const NamespacedTypename& other) :
	//	name_space(std::make_unique<NodeStructs::Typename>(*other.name_space.get())),
	//	name_in_name_space(std::make_unique<NodeStructs::Typename>(*other.name_in_name_space.get())) {}

	///*NamespacedTypename::operator=NamespacedTypename(const NamespacedTypename& other) {
	//	this->name_space
	//}*/

	//NamespacedTypename::NamespacedTypename(std::unique_ptr<Typename>&& name_space, std::unique_ptr<Typename>&& name_in_name_space) :
	//	name_space(std::move(name_space)), name_in_name_space(std::move(name_in_name_space)) {}

	bool NamespacedTypename::operator==(const NamespacedTypename& other) const {
		return *name_space == *other.name_space && *name_in_name_space == *other.name_in_name_space;
	}

	bool NamespacedTypename::operator==(const Typename& other) const {
		return std::visit(
			overload(
				[&](const NamespacedTypename& t) {
					return (*this) == t;
				},
				[](const auto&) {
					return false;
				}
			),
			other
		);
	}

	bool BaseTypename::operator==(const BaseTypename& other) const {
		return type == other.type;
	}

	bool BaseTypename::operator==(const Typename& other) const {
		return std::visit(
			overload(
				[&](const BaseTypename& t) {
					return (*this) == t;
				},
				[](const auto&) {
					return false;
				}
			),
			other
		);
	}
	//Expression::Expression(const Expression& other) : expression(std::make_unique<Expression>(*other.expression.get())) {}
}