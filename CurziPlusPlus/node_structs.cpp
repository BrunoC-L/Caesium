#include "node_structs.h"

namespace NodeStructs {
	bool TemplatedTypename::operator==(const TemplatedTypename& other) const {
		return type.get() == other.type.get() && templated_with == other.templated_with;
	}

	bool TemplatedTypename::operator==(const Typename& other) const {
		return std::visit(
			overload(overload_default_error,
				[&](const TemplatedTypename& t) {
					return operator==(t);
				},
				[](const NamespacedTypename&) {
					return false;
				},
				[](const BaseTypename&) {
					return false;
				}
			),
			other
		);
	}

	bool NamespacedTypename::operator==(const NamespacedTypename& other) const {
		return name_space.get() == other.name_space.get() && name_in_name_space.get() == other.name_in_name_space.get();
	}

	bool NamespacedTypename::operator==(const Typename& other) const {
		return std::visit(
			overload(overload_default_error,
				[](const TemplatedTypename&) {
					return false;
				},
				[&](const NamespacedTypename& t) {
					return operator==(t);
				},
				[](const BaseTypename&) {
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
			overload(overload_default_error,
				[](const TemplatedTypename&) {
					return false;
				},
				[](const NamespacedTypename&) {
					return false;
				},
				[&](const BaseTypename& t) {
					return operator==(t);
				}
			),
			other
		);
	}
}
