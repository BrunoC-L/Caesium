#include "helpers.hpp"

rule_info rule_info_stub_no_throw() {
	return rule_info{
		.file_name = "stub:/",
		.content = "stub",
		.beg = { 0, 0 },
		.end = { 0, 0 }
	};
}

rule_info rule_info_language_element(std::string s) {
	return rule_info{
		.file_name = "caesium:/",
		.content = std::move(s),
		.beg = { 0, 0 },
		.end = { 0, 0 }
	};
}

NodeStructs::Expression make_expression(NodeStructs::Expression::vt expr, rule_info info) {
	return { std::move(expr), std::move(info)};
}

NodeStructs::Typename make_typename(NodeStructs::Typename::vt tn, Optional<NodeStructs::ParameterCategory> cat, rule_info info) {
	return { std::move(tn), std::move(cat), std::move(info)};
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
	return e.info.content;
}

const std::string& original_representation(
	const NodeStructs::Expression& e
) {
	return e.info.content;
}

bool primitives_assignable(const NodeStructs::PrimitiveType& parameter, const NodeStructs::PrimitiveType& argument) {
	using vt = std::variant<
		NodeStructs::PrimitiveType::NonValued<std::string>,
		NodeStructs::PrimitiveType::NonValued<double>,
		NodeStructs::PrimitiveType::NonValued<int>,
		NodeStructs::PrimitiveType::NonValued<bool>,
		NodeStructs::PrimitiveType::NonValued<NodeStructs::void_t>,
		NodeStructs::PrimitiveType::NonValued<char>,
		NodeStructs::PrimitiveType::NonValued<NodeStructs::empty_optional_t>,

		NodeStructs::PrimitiveType::Valued<std::string>,
		NodeStructs::PrimitiveType::Valued<double>,
		NodeStructs::PrimitiveType::Valued<int>,
		NodeStructs::PrimitiveType::Valued<bool>,
		NodeStructs::PrimitiveType::Valued<NodeStructs::void_t>,
		NodeStructs::PrimitiveType::Valued<char>,
		NodeStructs::PrimitiveType::Valued<NodeStructs::empty_optional_t>
	>;
	constexpr unsigned diff = 7; // observe how the beginning indices are NonValued<T> and index + 7 is Valued<T>

	const vt& param = parameter.value._value;
	const vt& arg = argument.value._value;

	if (param.index() == arg.index())
		return true;

	if (param.index() + diff == arg.index()) // if param is nonvalued and param + diff matches valued arg, thats ok
		return true;
}
