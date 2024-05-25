#pragma once
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <set>
#include "../utility/expected.hpp"
#include "../utility/enumerate.hpp"

#include "node_structs.hpp"

struct variable_info {
	NodeStructs::ValueCategory value_category;
	NodeStructs::MetaType type;
};
using variables_t = std::map<std::string, std::vector<variable_info>>;
using transpile_declaration_definition_t = expected<std::pair<std::string, std::string>>;

struct type_information{
	NodeStructs::MetaType type;
	std::string representation;
};

struct non_type_information{
	NodeStructs::ExpressionType type;
	std::string representation;
	NodeStructs::ValueCategory value_category;
};

using expression_information = std::variant<type_information, non_type_information>;
using transpile_t = expected<std::string>;
using transpile_t2 = expected<expression_information>;

struct Namespace {
	std::string name;

	template <typename T> using map_to_vec = std::map<std::string, std::vector<T>>;

	map_to_vec<NodeStructs::Function> functions;
	map_to_vec<NodeStructs::Function> functions_using_auto;

	map_to_vec<NodeStructs::Type> types;
	map_to_vec<NodeStructs::Interface> interfaces;

	map_to_vec<NodeStructs::Template> templates;

	map_to_vec<NodeStructs::Block> blocks;
	std::map<std::string, NodeStructs::Typename> aliases;
	map_to_vec<NodeStructs::Enum> enums;

	std::map<std::string, Namespace> namespaces;
	map_to_vec<NodeStructs::Builtin> builtins;
};

inline auto copy(const Namespace& ns) {
	return copy11(ns);
}

struct transpilation_state {
	Namespace global_namespace;
	unsigned current_variable_unique_id = 1;

	transpilation_state(const transpilation_state& other) = delete;
	transpilation_state(transpilation_state&& other) = delete;
	transpilation_state(
		Namespace&& global_namespace
	) : global_namespace(std::move(global_namespace)) {}

	// shouldnt those be pointers...?
	std::set<NodeStructs::Function> traversed_functions;
	std::set<NodeStructs::Type> traversed_types;
	std::set<NodeStructs::Interface> traversed_interfaces;

	std::set<NodeStructs::Function> functions_to_transpile;
	std::set<NodeStructs::Type> types_to_transpile;
	std::set<NodeStructs::Interface> interfaces_to_transpile;
	std::set<NodeStructs::Enum> enums_to_transpile;

	std::map<NodeStructs::Typename, std::vector<NodeStructs::MetaType>> interface_symbol_to_members;
};

struct transpilation_state_with_indent {
	transpilation_state& state;
	size_t indent;

	transpilation_state_with_indent indented() {
		return { state, indent + 1 };
	}

	transpilation_state_with_indent unindented() {
		return { state, 0 };
	}
};

template <size_t token>
static constexpr std::string _symbol_as_text() {
	if constexpr (token == TOKENS::ASTERISK) return "*";
	if constexpr (token == TOKENS::SLASH) return "/";
	if constexpr (token == TOKENS::PERCENT) return "%";
	if constexpr (token == TOKENS::EQUAL) return "=";
	if constexpr (token == TOKENS::DASH) return "-";
	if constexpr (token == TOKENS::PLUS) return "+";
	if constexpr (token == TOKENS::NOT) return "!";
	if constexpr (token == TOKENS::EQUALEQUAL) return "==";
	if constexpr (token == TOKENS::NEQUAL) return "!=";
	if constexpr (token == TOKENS::LTQ) return "<";
	if constexpr (token == TOKENS::GTQ) return ">";
	if constexpr (token == TOKENS::LTEQ) return "<=";
	if constexpr (token == TOKENS::GTEQ) return ">=";
}

template <size_t token>
std::string symbol_as_text(Token<token>) {
	return _symbol_as_text<token>();
}

template <size_t... tokens>
std::string symbol_variant_as_text(const std::variant<Token<tokens>...>& token) {
	return std::visit(
		[&](const auto& tk) { return symbol_as_text(tk); },
		token
	);
}

static std::string indent(size_t n) {
	std::string res;
	res.reserve(n);
	for (size_t i = 0; i < n; ++i)
		res += '\t';
	return res;
};

static NodeStructs::ValueCategory argument_category_to_value_category(const NodeStructs::ArgumentCategory& cat) {
	return std::visit(overload(
		[](const NodeStructs::Reference&) -> NodeStructs::ValueCategory {
			return NodeStructs::Reference{};
		},
		[](const NodeStructs::MutableReference&) -> NodeStructs::ValueCategory {
			return NodeStructs::MutableReference{};
		},
		[](const NodeStructs::Move&) -> NodeStructs::ValueCategory {
			return NodeStructs::Value{};
		}
	), cat._value);
}

std::optional<error> validate_templates(const std::vector<NodeStructs::Template>& templates);

static NodeStructs::ValueCategory argument_category_optional_to_value_category(const std::optional<NodeStructs::ArgumentCategory>& cat) {
	if (cat.has_value())
		return argument_category_to_value_category(cat.value());
	else
		return NodeStructs::Value{};
}

transpile_t transpile(const std::vector<NodeStructs::File>& project);

transpile_declaration_definition_t transpile_main(
	transpilation_state_with_indent state,
	const NodeStructs::Function& fn
);

transpile_declaration_definition_t transpile(
	transpilation_state_with_indent state,
	const NodeStructs::Function& fn
);

std::optional<error> stack(
	transpilation_state_with_indent state,
	variables_t& variables,
	const std::vector<NodeStructs::FunctionParameter>& parameters
);

transpile_t transpile(
	transpilation_state_with_indent state,
	variables_t& variables,
	const std::vector<NodeStructs::Statement>& statements,
	const NodeStructs::MetaType& expected_return_type
);

transpile_t transpile(
	transpilation_state_with_indent state,
	variables_t& variables,
	const NodeStructs::Statement& statement,
	const NodeStructs::MetaType& expected_return_type
);

transpile_declaration_definition_t transpile(
	transpilation_state_with_indent state,
	const NodeStructs::Type& type
);

transpile_declaration_definition_t transpile(
	transpilation_state_with_indent state,
	const NodeStructs::Interface& interface
);

transpile_t transpile(
	transpilation_state_with_indent state,
	const std::vector<NodeStructs::FunctionParameter>& parameters
);

std::vector<NodeStructs::MetaType> decompose_type(
	transpilation_state_with_indent state,
	const NodeStructs::MetaType& type
);

//std::optional<error> add_decomposed_for_iterator_variables(
//	transpilation_state_with_indent state,
//	const std::vector<Variant<NodeStructs::VariableDeclaration, std::string>>& iterators,
//	const NodeStructs::MetaType& it_type
//);

std::optional<error> add_for_iterator_variables(
	transpilation_state_with_indent state,
	variables_t& variables,
	const std::vector<Variant<NodeStructs::VariableDeclaration, std::string>>& iterators,
	const NodeStructs::MetaType& it_type
);

transpile_t transpile_arg(
	transpilation_state_with_indent state,
	variables_t& variables,
	const NodeStructs::FunctionArgument& arg
);

transpile_t transpile_args(
	transpilation_state_with_indent state,
	variables_t& variables,
	const std::vector<NodeStructs::FunctionArgument>& args
);

transpile_t transpile_expressions(
	transpilation_state_with_indent state,
	const std::vector<NodeStructs::Expression>& args
);

transpile_t transpile_typenames(
	transpilation_state_with_indent state,
	const std::vector<NodeStructs::Typename>& args
);

NodeStructs::MetaType iterator_type(
	transpilation_state_with_indent state,
	const NodeStructs::MetaType& type
);

std::string template_name(
	std::string original_name,
	const std::vector<std::string>& args
);

std::string template_name(
	std::string original_name,
	const std::vector<NodeStructs::Expression>& arguments
);

bool is_assignable_to(
	transpilation_state_with_indent state,
	const NodeStructs::MetaType& parameter,
	const NodeStructs::MetaType& argument
);

transpile_t expr_to_printable(
	transpilation_state_with_indent state,
	variables_t& variables,
	const NodeStructs::Expression& expr
);

bool uses_auto(const NodeStructs::Function& fn);
bool uses_auto(const NodeStructs::FunctionParameter& param);
bool uses_auto(const NodeStructs::Statement& param);
bool uses_auto(const NodeStructs::Typename& t);

//#include "../type_visitor/transpile_type_visitor.hpp"
#include "../type_visitor/traverse_type_visitor.hpp"
#include "../type_visitor/type_of_function_like_call_with_args_visitor.hpp"
#include "../type_visitor/type_of_postfix_member_visitor.hpp"
#include "../type_visitor/transpile_member_call_visitor.hpp"
#include "../type_visitor/typename_of_type_visitor.hpp"
#include "../type_visitor/type_of_resolution_operator.hpp"

#include "../expression_visitor/expression_for_template_visitor.hpp"
#include "../expression_visitor/transpile_expression_visitor.hpp"
#include "../expression_visitor/expression_original_representation_visitor.hpp"

#include "../statement_visitor/transpile_statement_visitor.hpp"

#include "../typename_visitor/transpile_typename_visitor.hpp"
#include "../typename_visitor/type_of_typename_visitor.hpp"
#include "../typename_visitor/type_template_of_typename_visitor.hpp"
#include "../typename_visitor/typename_for_template_visitor.hpp"
#include "../typename_visitor/typename_original_representation_visitor.hpp"


expected<NodeStructs::Function> realise_function_using_auto(
	transpilation_state_with_indent state,
	const NodeStructs::Function& fn_using_auto,
	const std::vector<NodeStructs::MetaType>& arg_types
);

NodeStructs::Typename typename_of_primitive(const NodeStructs::PrimitiveType& primitive_t);

struct Arrangement {
	std::reference_wrapper<const NodeStructs::Template> tmpl;
	std::vector<size_t> arg_placements;
	/*std::weak_ordering operator<=>(const Arrangement& other) {
		return cmp(arg_placements, other.arg_placements);
	}*/
};

expected<Arrangement> find_best_template(
	const std::vector<NodeStructs::Template>& templates,
	const std::vector<NodeStructs::Expression>& args
);

expected<Arrangement> find_best_template(
	const std::vector<NodeStructs::Template>& templates,
	const std::vector<NodeStructs::Typename>& args
);

expected<std::optional<const NodeStructs::Function*>> find_best_function(
	transpilation_state_with_indent state,
	const std::string& name,
	const Namespace& space,
	const std::vector<NodeStructs::MetaType>& arg_types
);
