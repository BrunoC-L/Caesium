#include "../core/toCPP.hpp"

using T = transpile_builtin_call_with_args_visitor;
using R = T::R;

R T::operator()(const Realised::Builtin::builtin_compile_time_error& b) {
	return error{
		"user error",
		"user activated compile_time_error encountered"
	};
}

R T::operator()(const Realised::Builtin::builtin_typeof& b) {
	NOT_IMPLEMENTED;
}

R T::operator()(const Realised::Builtin::builtin_type_list& b) {
	NOT_IMPLEMENTED;
}

R T::operator()(const Realised::Builtin::builtin_exit& b) {
	if (args.size() != 1)
		NOT_IMPLEMENTED;
	auto t = transpile_expression(state, variables, args.at(0).expr);
	return_if_error(t);
	if (!holds<non_type_information>(t.value()))
		NOT_IMPLEMENTED;
	auto assigned = assigned_to(state, variables, int_metatype, get<non_type_information>(t.value()).type);
	return caesium_lib::variant::visit(
		std::move(assigned),
		overload(
			[&](const not_assignable&) -> R {
				return error{
					"user error",
					"exit() must be called with an integer exit code"
				};
			},
			[&](const directly_assignable&) -> R {
				return expression_information{ non_type_information{
					.type = copy(void_metatype),
					.representation = "exit(" + get<non_type_information>(std::move(t).value()).representation + ")",
					.value_category = NodeStructs::Value{}
				} };
			},
			[&](requires_conversion rc) -> R {
				auto converted = rc.converter(state, variables, args.at(0).expr);
				return_if_error(converted);
				if (!holds<non_type_information>(converted.value()))
					NOT_IMPLEMENTED;
				return expression_information{ non_type_information{
					.type = copy(void_metatype),
					.representation = "exit(" + get<non_type_information>(std::move(converted).value()).representation + ")",
					.value_category = NodeStructs::Value{}
				} };
			}
		)
	);
}

R T::operator()(const Realised::Builtin::builtin_print& b) {
	std::stringstream ss;
	ss << "(Void)(std::cout";
	for (const auto& [_, arg] : args) {
		auto expr_s = expr_to_printable(state, variables, arg);
		return_if_error(expr_s);
		ss << " << " << expr_s.value();
	}
	ss << ")";
	return expression_information{ non_type_information{
		.type = Realised::MetaType{ Realised::PrimitiveType{ Realised::PrimitiveType::NonValued<Realised::void_t>{} } },
		.representation = ss.str(),
		.value_category = NodeStructs::Value{}
	} };
}

R T::operator()(const Realised::Builtin::builtin_println& b) {
	std::stringstream ss;
	ss << "(Void)(std::cout";
	for (const auto& [_, arg] : args) {
		auto expr_s = expr_to_printable(state, variables, arg);
		return_if_error(expr_s);
		ss << " << " << expr_s.value();
	}
	ss << " << \"\\n\"";
	ss << ")";
	return expression_information{ non_type_information{
		.type = Realised::MetaType{ Realised::PrimitiveType{ Realised::PrimitiveType::NonValued<Realised::void_t>{} } },
		.representation = ss.str(),
		.value_category = NodeStructs::Value{}
	} };
}

R T::operator()(const Realised::Builtin::builtin_vector& b) {
	NOT_IMPLEMENTED;
}

R T::operator()(const Realised::Builtin::builtin_set& b) {
	NOT_IMPLEMENTED;
}

R T::operator()(const Realised::Builtin::builtin_map& b) {
	NOT_IMPLEMENTED;
}

R T::operator()(const Realised::Builtin::builtin_union& b) {
	NOT_IMPLEMENTED;
}
