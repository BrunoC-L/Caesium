#include "helpers.hpp"

caesium_source_location rule_info_stub_no_throw() {
	return caesium_source_location{
		.file_name = "stub:/",
		.content = "stub",
		.beg = { 0, 0 },
		.end = { 0, 0 }
	};
}

caesium_source_location rule_info_language_element(std::string s) {
	return caesium_source_location{
		.file_name = "caesium:/",
		.content = std::move(s),
		.beg = { 0, 0 },
		.end = { 0, 0 }
	};
}

NodeStructs::Expression make_expression(NodeStructs::Expression::vt expr, caesium_source_location info) {
	return { std::move(expr), std::move(info)};
}

NodeStructs::Typename make_typename(NodeStructs::Typename::vt tn, Optional<NodeStructs::ValueCategory> cat, caesium_source_location info) {
	return { std::move(tn), std::move(cat), std::move(info) };
}

NodeStructs::NameSpace make_namespace(
	std::string name,
	std::optional<NodeStructs::Typename> name_space,
	std::vector<NodeStructs::Function> functions,
	std::vector<NodeStructs::Type> types,
	std::vector<NodeStructs::Interface> interfaces,
	std::vector<NodeStructs::Template> templates,
	std::vector<NodeStructs::Block> blocks,
	std::vector<NodeStructs::Alias> aliases,
	std::vector<NodeStructs::Enum> enums,
	std::vector<NodeStructs::NameSpace> namespaces,
	caesium_source_location info
) {
	return NodeStructs::NameSpace{
		std::move(name),
		std::move(name_space),
		std::move(functions),
		std::move(types),
		std::move(interfaces),
		std::move(templates),
		std::move(blocks),
		std::move(aliases),
		std::move(enums),
		std::move(namespaces),
		std::move(info),
	};
}

Realised::MetaType make_type(Realised::MetaType::vt t) {
	return { std::move(t) };
}

Realised::Parameter make_parameter(Realised::MetaType t, NodeStructs::ValueCategory cat) {
	return { std::move(t), std::move(cat) };
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
