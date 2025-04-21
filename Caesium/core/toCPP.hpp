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
#include "../structured/helpers.hpp"
#include "realised.hpp"

struct variable_info {
	NodeStructs::ValueCategory value_category;
	Realised::MetaType type;
};
using variables_t = std::map<std::string, std::vector<variable_info>>;
using transpile_declaration_definition_t = expected<std::pair<std::string, std::string>>;

struct type_information{
	Realised::MetaType type;
	std::string representation;
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

using Namespace = Realised::NameSpace;

// shouldnt those be pointers...?
template <typename T>             using set = std::set<T, default_less_than>;
template <typename K, typename V> using map = std::map<K, V, default_less_than>;

template <typename T>
struct traversal {
	std::set<std::string> traversing;
	std::map<std::string, T> traversed;
	std::map<std::string, std::vector<std::string>> definitions;
	std::map<std::string, std::vector<std::string>> declarations;
};

struct transpilation_state {
	Namespace global_namespace;
	unsigned current_variable_unique_id = 1;

	transpilation_state(const transpilation_state& other) = delete;
	transpilation_state(transpilation_state&& other) = delete;
	transpilation_state(
		Namespace&& global_namespace
	) : global_namespace(std::move(global_namespace)) {}

	traversal<Realised::Function> functions_traversal;
	traversal<Realised::Type> types_traversal;
	traversal<Realised::Interface> interfaces_traversal;

	set<NodeStructs::Enum> enums_to_transpile;
	map<NodeStructs::Typename, std::vector<Realised::MetaType>> interface_symbol_to_members;

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

transpile_declaration_definition_t transpile_type(
	transpilation_state_with_indent state,
	const Realised::Type& type
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
	transpilation_state_with_indent state_,
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

#include "../type_visitor/traverse_type_visitor.hpp"
#include "../type_visitor/type_of_function_like_call_with_args_visitor.hpp"
#include "../type_visitor/type_of_postfix_member_visitor.hpp"
#include "../type_visitor/transpile_member_call_visitor.hpp"
#include "../type_visitor/typename_of_type_visitor.hpp"
#include "../type_visitor/type_of_resolution_operator.hpp"

#include "../expression_visitor/transpile_expression_visitor.hpp"

#include "../typename_visitor/transpile_typename_visitor.hpp"
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

expected<std::string> word_typename_or_expression_for_template(
	transpilation_state_with_indent state,
	variables_t& variables,
	const NodeStructs::WordTypenameOrExpression& value
);

expected<Realised::MetaType> type_of_typename(
	transpilation_state_with_indent state,
	variables_t& variables,
	const NodeStructs::WordTypenameOrExpression& tn_or_expr
);

expected<NodeStructs::Function> realise_function_using_auto(
	transpilation_state_with_indent state,
	const NodeStructs::Function& fn_using_auto,
	const std::vector<Realised::MetaType>& arg_types
);

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

//expected<std::optional<std::reference_wrapper<const NodeStructs::Function>>> find_best_function(
//	transpilation_state_with_indent state,
//	variables_t& variables,
//	const std::string& name,
//	const Namespace& space,
//	const std::vector<Realised::MetaType>& arg_types
//);

template <typename context>
transpile_t transpile(
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

expected<Realised::Type> realise(
	transpilation_state_with_indent state,
	const NodeStructs::Type& type
);

expected<Realised::Type> get_existing_realised_type(
	transpilation_state_with_indent state,
	const std::string& name,
	const std::optional<NodeStructs::NameSpace>& name_space
);
