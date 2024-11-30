#include "helpers.hpp"

rule_info rule_info_stub() {
	throw;
}

rule_info rule_info_stub_no_throw() {
	return rule_info{
		.file_name = "stub:/",
		.content = "",
		.beg = { 0, 0 },
		.end = { 0, 0 }
	};
}

NodeStructs::Expression make_expression(NodeStructs::Expression::vt expr, rule_info info) {
	return { std::move(expr), std::move(info) };
}

NodeStructs::Typename make_typename(NodeStructs::Typename::vt tn, Optional<NodeStructs::ParameterCategory> cat, rule_info info) {
	return { std::move(tn), std::move(cat), std::move(info) };
}

const std::string& original_representation(
	const NodeStructs::WordTypenameOrExpression& tn_or_expr
) {
	return std::visit([](const auto& e) -> const std::string& { return original_representation(e); }, tn_or_expr.value._value);
}

const std::string& original_representation(
	const std::string& e
) {
	return e;
}

const std::string& original_representation(
	const NodeStructs::Typename& e
) {
	return e.rule_info.content;
}

const std::string& original_representation(
	const NodeStructs::Expression& e
) {
	return e.rule_info.content;
}
