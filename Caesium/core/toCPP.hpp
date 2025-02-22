#pragma once
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <set>
#include <functional>
#include "../utility/expected.hpp"
#include "../utility/enumerate.hpp"
#include "../structured/node_structs.hpp"
#include "../structured/structurizer.hpp"
#include "../structured/helpers.hpp"

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
	NodeStructs::MetaType type;
	std::string representation;
	NodeStructs::ValueCategory value_category;
};

template <>
struct copy_t<non_type_information> {
	static non_type_information copy(const non_type_information& info) {
		return copy3(info);
	}
};

using expression_information = std::variant<type_information, non_type_information>;
using transpile_t = expected<std::string>;
using transpile_expression_information_t = expected<expression_information>;


struct Namespace {
	std::string name;

	template <typename T> using map_to_vec = std::map<std::string, std::vector<T>, default_less_than>;

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
	rule_info info = rule_info_stub<Namespace>();
};

template <>
struct copy_t<Namespace> {
	static Namespace copy(const Namespace& ns) {
		return copy12(ns);
	}
};

struct transpilation_state {
	Namespace global_namespace;
	unsigned current_variable_unique_id = 1;

	transpilation_state(const transpilation_state& other) = delete;
	transpilation_state(transpilation_state&& other) = delete;
	transpilation_state(
		Namespace&& global_namespace
	) : global_namespace(std::move(global_namespace)) {}

	// shouldnt those be pointers...?
	std::set<NodeStructs::Function, default_less_than> traversed_functions;
	std::set<NodeStructs::Type, default_less_than> traversed_types;
	std::set<NodeStructs::Interface, default_less_than> traversed_interfaces;

	std::set<NodeStructs::Function, default_less_than> functions_to_transpile;
	std::vector<NodeStructs::Type> types_to_transpile;
	std::set<NodeStructs::Interface, default_less_than> interfaces_to_transpile;
	std::set<NodeStructs::Enum, default_less_than> enums_to_transpile;

	std::map<NodeStructs::Typename, std::vector<NodeStructs::MetaType>, default_less_than> interface_symbol_to_members;
	std::set<std::pair<std::string, std::string>> aliases_to_transpile;
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

template <token_t token>
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
	if constexpr (token == TOKENS::LT) return "<";
	if constexpr (token == TOKENS::GT) return ">";
	if constexpr (token == TOKENS::LTE) return "<=";
	if constexpr (token == TOKENS::GTE) return ">=";
	// compilation error if control flow reaches here
}

template <token_t token>
std::string symbol_as_text(Token<token>) {
	return _symbol_as_text<token>();
}

template <token_t... tokens>
std::string symbol_variant_as_text(const std::variant<Token<tokens>...>& token) {
	return std::visit(
		[&](const auto& tk) { return symbol_as_text(tk); },
		token
	);
}

static inline std::string indent(size_t n) {
	std::string res;
	res.reserve(n);
	for (size_t i = 0; i < n; ++i)
		res += '\t';
	return res;
};

std::optional<error> validate_templates(const std::vector<NodeStructs::Template>& templates);

//static NodeStructs::ValueCategory argument_category_to_value_category(const NodeStructs::ArgumentCategory& cat) {
//	return std::visit(overload(
//		[](const NodeStructs::Reference&) -> NodeStructs::ValueCategory {
//			return NodeStructs::Reference{};
//		},
//		[](const NodeStructs::MutableReference&) -> NodeStructs::ValueCategory {
//			return NodeStructs::MutableReference{};
//		},
//		[](const NodeStructs::Move&) -> NodeStructs::ValueCategory {
//			return NodeStructs::Value{};
//		}
//	), cat._value);
//}

//static NodeStructs::ValueCategory argument_category_optional_to_value_category(const std::optional<NodeStructs::ArgumentCategory>& cat) {
//	if (cat.has_value())
//		return argument_category_to_value_category(cat.value());
//	else
//		return NodeStructs::Value{};
//}

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

template <typename context>
transpile_t transpile(
	transpilation_state_with_indent state,
	variables_t& variables,
	const NodeStructs::Statement<context>& statement,
	const NodeStructs::MetaType& expected_return_type
);

transpile_declaration_definition_t transpile_type(
	transpilation_state_with_indent state,
	const NodeStructs::Type& type
);

transpile_declaration_definition_t transpile(
	transpilation_state_with_indent state,
	const NodeStructs::Interface& interface
);

transpile_t transpile(
	transpilation_state_with_indent state,
	variables_t& variables,
	const std::vector<NodeStructs::FunctionParameter>& parameters
);

std::vector<NodeStructs::MetaType> decompose_type(
	transpilation_state_with_indent state,
	const NodeStructs::MetaType& type
);

std::optional<error> add_for_iterator_variables(
	transpilation_state_with_indent state,
	variables_t& variables,
	const std::vector<Variant<NodeStructs::VariableDeclaration, std::string>>& iterators,
	const NodeStructs::MetaType& it_type
);

transpile_expression_information_t transpile_arg(
	transpilation_state_with_indent state,
	variables_t& variables,
	const NodeStructs::FunctionArgument& arg
);

transpile_t transpile_typenames(
	transpilation_state_with_indent state,
	variables_t& variables,
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
	const std::vector<NodeStructs::WordTypenameOrExpression>& arguments
);

struct not_assignable {};
struct directly_assignable {};
struct requires_conversion {
	std::move_only_function<transpile_expression_information_t(transpilation_state_with_indent, variables_t&, const NodeStructs::Expression&)> converter;
};

Variant<not_assignable, directly_assignable, requires_conversion> assigned_to(
	transpilation_state_with_indent state_,
	variables_t& variables,
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
bool uses_auto(const NodeStructs::Typename& t);
bool uses_auto(const NodeStructs::Expression& t);

//#include "../type_visitor/transpile_type_visitor.hpp"
#include "../type_visitor/traverse_type_visitor.hpp"
#include "../type_visitor/type_of_function_like_call_with_args_visitor.hpp"
#include "../type_visitor/type_of_postfix_member_visitor.hpp"
#include "../type_visitor/transpile_member_call_visitor.hpp"
#include "../type_visitor/typename_of_type_visitor.hpp"
#include "../type_visitor/type_of_resolution_operator.hpp"

//#include "../expression_visitor/expression_for_template_visitor.hpp"
#include "../expression_visitor/transpile_expression_visitor.hpp"

//#include "../statement_visitor/transpile_statement_visitor.hpp"
#include "../statement_visitor/transpile_statement.hpp"

#include "../typename_visitor/transpile_typename_visitor.hpp"
#include "../typename_visitor/type_of_typename_visitor.hpp"
#include "../typename_visitor/type_template_of_typename_visitor.hpp"
//#include "../typename_visitor/typename_for_template_visitor.hpp"

expected<std::string> word_typename_or_expression_for_template(
	transpilation_state_with_indent state,
	variables_t& variables,
	const NodeStructs::WordTypenameOrExpression& value
);

expected<NodeStructs::MetaType> type_of_typename(
	transpilation_state_with_indent state,
	variables_t& variables,
	const NodeStructs::WordTypenameOrExpression& tn_or_expr
);

expected<NodeStructs::Function> realise_function_using_auto(
	transpilation_state_with_indent state,
	const NodeStructs::Function& fn_using_auto,
	const std::vector<NodeStructs::MetaType>& arg_types
);

NodeStructs::Typename typename_of_primitive(const NodeStructs::PrimitiveType& primitive_t);

struct Arrangement {
	std::reference_wrapper<const NodeStructs::Template> tmpl;
	std::vector<size_t> arg_placements;
};

expected<Arrangement> find_best_template(
	transpilation_state_with_indent state,
	variables_t& variables,
	const std::vector<NodeStructs::Template>& templates,
	const std::vector<NodeStructs::WordTypenameOrExpression>& args
);

expected<std::optional<const NodeStructs::Function*>> find_best_function(
	transpilation_state_with_indent state,
	variables_t& variables,
	const std::string& name,
	const Namespace& space,
	const std::vector<NodeStructs::MetaType>& arg_types
);

template <typename context>
transpile_t transpile(
	transpilation_state_with_indent state,
	variables_t& variables,
	const std::vector<NodeStructs::Statement<context>>& statements,
	const NodeStructs::MetaType& expected_return_type
) {
	std::stringstream ss;
	for (const NodeStructs::Statement<context>& statement : statements) {
		auto k = transpile_statement(state, variables, expected_return_type, statement);
		if (k.has_value())
			ss << indent(state.indent) << k.value();
		else
			return k.error();
	}

	return ss.str();
}

template <typename context>
bool uses_auto(const NodeStructs::Statement<context>& statement) {
	if (holds<NodeStructs::CompileTimeStatement<context>>(statement))
		return false;
	if constexpr (std::is_same_v<context, function_context>) {
		const Variant<NodeStructs::RunTimeStatement>& st1 = get<Variant<NodeStructs::RunTimeStatement>>(statement);
		const NodeStructs::RunTimeStatement& st = get<NodeStructs::RunTimeStatement>(st1);
		if (holds<NodeStructs::VariableDeclarationStatement<context>>(st)) {
			const auto& var_decl = get<NodeStructs::VariableDeclarationStatement<context>>(st);
			return uses_auto(var_decl.type);
		}
		else
			return false;
	}
	NOT_IMPLEMENTED;
	// todo recursive impl and check for variable declarations and for statements
}
