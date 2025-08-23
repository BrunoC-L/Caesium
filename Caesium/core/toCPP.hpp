#pragma once
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <set>
#include <functional>
#include "../utility/tag_type.hpp"
#include "../utility/expected.hpp"
#include "../utility/enumerate.hpp"
#include "../structured/node_structs.hpp"
#include "../structured/structurizer.hpp"
#include "realised.hpp"

static const auto void_metatype = Realised::MetaType{ Realised::PrimitiveType{ Realised::PrimitiveType::Void{} } };
static const auto int_metatype = Realised::MetaType{ Realised::PrimitiveType{ Realised::PrimitiveType::Int{} } };

struct variable_info {
	NodeStructs::ValueCategory value_category;
	Realised::MetaType type;
};

template <>
struct copy_t<variable_info> {
	static variable_info copy(const variable_info& info) {
		return copy2(info);
	}
};

using variables_t = std::map<std::string, std::vector<variable_info>>;
using transpile_declaration_definition_t = expected<std::pair<std::string, std::string>>;

struct type_information{
	Realised::MetaType type;
	std::string representation;
};

template <>
struct copy_t<type_information> {
	static type_information copy(const type_information& info) {
		return copy2(info);
	}
};

struct non_type_information{
	Realised::MetaType type;
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

// shouldnt those be pointers...?
template <typename T>             using set = std::set<T, default_less_than>;
template <typename K, typename V> using map = std::map<K, V, default_less_than>;

template <typename T>
struct traversal {
	std::set<std::string> traversing;
	std::map<std::string, T> traversed;
	std::vector<std::string> definitions;
	std::vector<std::string> declarations;
};

struct transpilation_state {
	NodeStructs::NameSpace global_namespace;
	unsigned current_variable_unique_id = 1;

	transpilation_state(const transpilation_state& other) = delete;
	transpilation_state(transpilation_state&& other) = delete;
	transpilation_state(NodeStructs::NameSpace global_namespace) : global_namespace(std::move(global_namespace)) {}

	traversal<Realised::Function> functions_traversal;
	std::map<std::string, Realised::MetaType> functions_return_types; // to get between traversing and traversed (needed for recursion)
	traversal<Realised::Type> types_traversal;
	traversal<Realised::Interface> interfaces_traversal;

	std::map<std::string, Realised::MetaType> types;

	std::map<std::string, NodeStructs::Enum> enums_to_transpile;
	map<std::string, std::vector<std::string>> interface_to_members;

	std::map<std::string, std::string> aliases_to_transpile;
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

transpile_t transpile(const std::vector<NodeStructs::File>& project);

transpile_t name_of_namespace(const NodeStructs::NameSpace&);
transpile_t name_of_namespace(const NodeStructs::Typename&);

std::optional<error> realise_main(
	transpilation_state_with_indent state,
	const NodeStructs::Function& fn
);

transpile_declaration_definition_t transpile(
	transpilation_state_with_indent state,
	const NodeStructs::Type& type
);

transpile_declaration_definition_t transpile(
	transpilation_state_with_indent state,
	const NodeStructs::Interface& interface
);

std::vector<Realised::MetaType> decompose_type(
	transpilation_state_with_indent state,
	const Realised::MetaType& type
);

std::optional<error> add_for_iterator_variables(
	transpilation_state_with_indent state,
	variables_t& variables,
	const std::vector<Variant<NodeStructs::VariableDeclaration, std::string>>& iterators,
	const Realised::MetaType& it_type
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

Realised::MetaType iterator_type(
	transpilation_state_with_indent state,
	const Realised::MetaType& type
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
	transpilation_state_with_indent state,
	variables_t& variables,
	const Realised::MetaType& parameter,
	const Realised::MetaType& argument
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

#include "../typename_visitor/realise_typename_visitor.hpp"
#include "../type_visitor/type_of_function_like_call_with_args_visitor.hpp"
#include "../type_visitor/type_of_postfix_member_visitor.hpp"
#include "../type_visitor/transpile_member_call_visitor.hpp"
#include "../type_visitor/name_of_type_visitor.hpp"
#include "../type_visitor/type_of_resolution_operator.hpp"

#include "../builtin_visitor/transpile_builtin_call_with_args_visitor.hpp"

#include "../expression_visitor/transpile_expression_visitor.hpp"

#include "../typename_visitor/type_of_typename_visitor.hpp"
#include "../typename_visitor/type_template_of_typename_visitor.hpp"

transpile_t transpile_statement(
	transpilation_state_with_indent state,
	variables_t& variables,
	const Realised::MetaType& expected_return_type,
	const NodeStructs::Statement<function_context>& statement
);

transpile_t transpile_statement(
	transpilation_state_with_indent state,
	variables_t& variables,
	const Realised::MetaType& expected_return_type,
	const NodeStructs::Statement<type_context>& statement
);

transpile_t transpile_statement(
	transpilation_state_with_indent state,
	variables_t& variables,
	const Realised::MetaType& expected_return_type,
	const NodeStructs::Statement<top_level_context>& statement
);

transpile_t word_typename_or_expression_for_template(
	transpilation_state_with_indent state,
	variables_t& variables,
	const NodeStructs::WordTypenameOrExpression& value
);

transpile_expression_information_t type_of_typename(
	transpilation_state_with_indent state,
	variables_t& variables,
	const NodeStructs::WordTypenameOrExpression& tn_or_expr
);

std::string name_of_primitive(const Realised::PrimitiveType& primitive_t);
NodeStructs::Typename typename_of_primitive(const Realised::PrimitiveType& primitive_t);

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

expected<Realised::Function> realise_function(
	transpilation_state_with_indent state,
	const NodeStructs::Function& function,
	const std::string& fkey,
	const std::vector<expression_information>& args
);

transpile_expression_information_t produce_call(
	transpilation_state_with_indent state,
	variables_t& variables,
	const std::string& fkey,
	const Realised::MetaType& return_type,
	const std::vector<NodeStructs::FunctionParameter>& parameters,
	const std::vector<expression_information>& args
);

transpile_expression_information_t realise_function_and_produce_call(
	transpilation_state_with_indent state,
	variables_t& variables,
	const Realised::FunctionType& fn,
	const NodeStructs::NameSpace& space,
	const std::vector<expression_information>& args
);

template <typename context>
transpile_t transpile_statements(
	transpilation_state_with_indent state,
	variables_t& variables,
	const std::vector<NodeStructs::Statement<context>>& statements,
	const Realised::MetaType& expected_return_type
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
	// todo recursive impl and check for variable declarations and for statements
	NOT_IMPLEMENTED;
}

variables_t make_base_variables();

std::string_view name_of_builtin(const Realised::Builtin& builtin);

NodeStructs::ValueCategory optional_parameter_category_to_value_category(const Optional<NodeStructs::ValueCategory>& opt_cat);
NodeStructs::ValueCategory optional_parameter_category_to_value_category(const std::optional<NodeStructs::ValueCategory>& opt_cat);
NodeStructs::ValueCategory parameter_category_to_value_category(const NodeStructs::ValueCategory& cat);

#include "realise.hpp"

NodeStructs::ValueCategory category_of_word_typename_or_expression(
	transpilation_state_with_indent state,
	variables_t& variables,
	const NodeStructs::WordTypenameOrExpression& wte
);

caesium_source_location info_of_word_typename_or_expression(
	transpilation_state_with_indent state,
	variables_t& variables,
	const NodeStructs::WordTypenameOrExpression& wte
);

template <typename T>
void add_to_traversal_if_missing(
	traversal<T>& traversal,
	const std::string& name,
	const T& to_insert
) {
	if (traversal.traversing.contains(name) || traversal.traversed.contains(name))
		return;
	traversal.traversed.insert(name, copy(to_insert));
}

std::optional<
	std::pair<
		std::reference_wrapper<const NodeStructs::Function>,
		std::vector<Variant<directly_assignable, requires_conversion>>
	>
> find_best_function(
	transpilation_state_with_indent state,
	variables_t& variables,
	const std::vector<NodeStructs::Function>& overload_set,
	const std::vector<expression_information>& args
);
