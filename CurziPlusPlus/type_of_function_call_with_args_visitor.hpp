#include "toCPP.h"
#include "type_variant_visitor.hpp"

struct type_of_member_function_like_call_with_args_visitor : TypeCategoryVisitor<type_of_member_function_like_call_with_args_visitor> {
	using TypeCategoryVisitor<type_of_member_function_like_call_with_args_visitor>::operator();

	variables_t& variables;
	const Named& named;
	const std::vector<NodeStructs::FunctionArgument>& args;

	using R = std::expected<std::pair<NodeStructs::ValueCategory, NodeStructs::TypeCategory>, user_error>;

	R operator()(const NodeStructs::Type& t) {
		return user_error{
			"Use of type like a function is prohibited. Type was `" + t.name + "`"
		}.unexpected();
	}
	R operator()(const NodeStructs::TypeTemplateInstance& t) {
		return user_error{
			"Use of type like a function is prohibited. Type was `" + t.type_template.get().templated.name + "`"
		}.unexpected();
	}
	R operator()(const NodeStructs::TypeAggregate& t) {
		return user_error{
			"Use of an aggregate like a function is prohibited. Aggregate was"
		}.unexpected();
	}
	R operator()(const NodeStructs::TypeType& t) {
		return user_error{
			"Use of a 'type' type like a function is prohibited, a 'type' type results from typeof(<some type>)"
		}.unexpected();
	}
	R operator()(const NodeStructs::TypeTemplateType t) {
		throw;
	}
	R operator()(const NodeStructs::FunctionType& t) {
		throw;
	}
	R operator()(const NodeStructs::FunctionTemplateType& t) {
		throw;
	}
	R operator()(const NodeStructs::UnionType& t) {
		throw;
	}
};
