#pragma once
#include "../core/realised.hpp"

template <typename T>
struct BuiltinVisitor {
	template <typename Self>
	auto operator()(this Self&& self, const Realised::Builtin& t) {
		return caesium_lib::variant::visit(
			t.builtin,
			[&](const auto& t) {
				return self(t);
			}
		);
	}
};

#define BuiltinVisitorDeclarations \
	 R operator()(const Realised::Builtin::builtin_compile_time_error& b);\
	 R operator()(const Realised::Builtin::builtin_typeof& b);\
	 R operator()(const Realised::Builtin::builtin_type_list& b);\
	 R operator()(const Realised::Builtin::builtin_exit& b);\
	 R operator()(const Realised::Builtin::builtin_print& b);\
	 R operator()(const Realised::Builtin::builtin_println& b);\
\
	 R operator()(const Realised::Builtin::builtin_vector& b);\
	 R operator()(const Realised::Builtin::builtin_set& b);\
	 R operator()(const Realised::Builtin::builtin_map& b);\
	 R operator()(const Realised::Builtin::builtin_union& b);
