#include "type_of_function_like_call_with_args_visitor.hpp"
#include "../type_of_typename_visitor.hpp"
#include "../type_of_expression_visitor.hpp"

using T = type_of_function_like_call_with_args_visitor;
using R = T::R;

R T::operator()(const NodeStructs::Type& t) {
	return error{
		"user error",
		"Use of type like a function is prohibited. Type was `" + t.name + "`"
	};
}

//R T::operator()(const NodeStructs::TypeTemplateInstanceType& t) {
//	return error{
//		"user error",
//		"Use of type like a function is prohibited. Type was `" + t.type_template.get().templated.name + "`"
//	};
//}

R T::operator()(const NodeStructs::AggregateType& t) {
	return error{
		"user error",
		"Use of an aggregate like a function is prohibited. Aggregate was"
	};
}

R T::operator()(const NodeStructs::TypeType& t) {
	return error{
		"user error",
		"Use of a 'type' type like a function is prohibited, a 'type' type results from typeof(<some type>)"
	};
}

R T::operator()(const NodeStructs::FunctionType& t) {
	if (args.size() != t.function.get().parameters.size()) {
		std::stringstream ss;
		ss << t.function.get().name << " has " << t.function.get().parameters.size() << " parameters but received " << args.size() << " arguments";
		return error{ "user error", ss.str() };
	}
	else {
		// if args apply... TODO
		return std::pair{ NodeStructs::Value{}, type_of_typename_visitor{ {}, state }(t.function.get().returnType).value() };
	}
	throw;
}

R T::operator()(const NodeStructs::InterfaceType& t) {
	throw;
}

R T::operator()(const NodeStructs::UnionType& t) {
	throw;
}

R T::operator()(const NodeStructs::VectorType& t) {
	throw;
}

R T::operator()(const NodeStructs::SetType& t) {
	throw;
}

R T::operator()(const NodeStructs::MapType& t) {
	throw;
}

R T::operator()(const NodeStructs::Template& tmpl) {
	if (tmpl.templated == "BUILTIN") {
		if (tmpl.name == "println") {
			return std::pair{ NodeStructs::Value{}, NodeStructs::UniversalType{ *state.state.named.types.at("Void").back() } };
		}
	}
}

R T::operator()(const NodeStructs::BuiltInType& t) {
	return std::visit(
		overload(
			[&](const auto& e) -> R {
				throw;
			},
			[&](const NodeStructs::BuiltInType::push_t& e) -> R {
				if (this->args.size() != 1)
					throw;
				auto arg_t = type_of_expression_visitor{ {}, state }(std::get<NodeStructs::Expression>(args.at(0)));
				return_if_error(arg_t);
				if (!is_assignable_to(state, e.container.value_type.get(), arg_t.value().second)) {
					throw;
				}
				return std::pair{
					NodeStructs::Value{},
					NodeStructs::UniversalType{
						std::reference_wrapper<const NodeStructs::Type>{
							*state.state.named.types.at("Void").back()
						}
					}
				};
			}
		),
		t.builtin
	);
	throw;
}

R T::operator()(const std::string&) {
	throw;
}

R T::operator()(const double&) {
	throw;
}

R T::operator()(const int&) {
	throw;
}

R T::operator()(const bool&) {
	throw;
}
