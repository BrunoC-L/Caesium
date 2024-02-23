#pragma once
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <set>
#include "../utility/expected.hpp"
#include <ranges>

#include "node_structs.hpp"

using variables_t = std::map<std::string, std::vector<std::pair<NodeStructs::ValueCategory, NodeStructs::UniversalType>>>;
using transpile_header_cpp_t = expected<std::pair<std::string, std::string>>;
using transpile_t = expected<std::string>;
struct type_and_representation {
	NodeStructs::UniversalType type;
	std::string representation;
};
using transpile_type_repr = expected<type_and_representation>;

struct Named {
	template <typename T> using map_to_vec = std::map<std::string, std::vector<T const*>>;

	map_to_vec<NodeStructs::Function> functions;
	map_to_vec<NodeStructs::Type> types;
	map_to_vec<NodeStructs::Interface> interfaces;
	map_to_vec<NodeStructs::Block> blocks;
	map_to_vec<NodeStructs::Template> templates;
	std::map<std::string, NodeStructs::Typename> type_aliases_typenames;
	std::map<std::string, NodeStructs::UniversalType> type_aliases;
	//map_to_vec<NodeStructs::Template<NodeStructs::Function>> function_templates;
	//map_to_vec<NodeStructs::Template<NodeStructs::Type>> type_templates;
	//map_to_vec<NodeStructs::Template<NodeStructs::Block>> block_templates;
};

struct transpilation_state {
	variables_t variables;
	Named named;
	std::vector<std::pair<std::string, std::string>> transpile_in_reverse_order;

	transpilation_state(const transpilation_state& other) = delete;
	transpilation_state(
		variables_t&& variables,
		Named&& named
	) : variables(std::move(variables)), named(std::move(named)) {}

	// shouldnt those be pointers...?
	std::set<NodeStructs::Function> traversed_functions;
	std::set<NodeStructs::Type> traversed_types;
	std::set<NodeStructs::Interface> traversed_interfaces;
	std::set<NodeStructs::Block> traversed_blocks;
	std::set<NodeStructs::Alias> traversed_type_aliases;
	std::map<NodeStructs::Typename, std::vector<NodeStructs::UniversalType>> interface_symbol_to_members;
};

struct transpilation_state_with_indent {
	transpilation_state& state;
	size_t indent = 0;

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
	if constexpr (token == TOKENS::TILDE) return "~";
	if constexpr (token == TOKENS::EQUALEQUAL) return "==";
	if constexpr (token == TOKENS::NEQUAL) return "!=";
	if constexpr (token == TOKENS::PLUSEQUAL) return "+=";
	if constexpr (token == TOKENS::MINUSEQUAL) return "-=";
	if constexpr (token == TOKENS::TIMESEQUAL) return "*=";
	if constexpr (token == TOKENS::DIVEQUAL) return "/=";
	if constexpr (token == TOKENS::MODEQUAL) return "%=";
	if constexpr (token == TOKENS::ANDEQUAL) return "&=";
	if constexpr (token == TOKENS::OREQUAL) return "|=";
	if constexpr (token == TOKENS::XOREQUAL) return "^=";
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

struct builtins {
	NodeStructs::Template builtin_set = { "Set" , { { "T", {} } }, "BUILTIN" };
	NodeStructs::Template builtin_vector = { "Vector" , { { "T", std::nullopt}}, "BUILTIN"};
	NodeStructs::Template builtin_map = { "Map" , { { "K", {} }, { "V", {} } }, "BUILTIN" };
	NodeStructs::Template builtin_push = { "push" , { { "Cnt", {} }, { "T", {} } }, "BUILTIN" }; // vec
	NodeStructs::Template builtin_insert = { "insert" , { { "Cnt", {} }, { "T", {} } }, "BUILTIN" }; // map or set

	NodeStructs::Template builtin_print = { "print" , { { "T", {} } }, "BUILTIN" };
	NodeStructs::Template builtin_println = { "println" , { { "T", {} } }, "BUILTIN" };

	NodeStructs::Type builtin_int = { "Int" };
	NodeStructs::Type builtin_bool = { "Bool" };
	NodeStructs::Type builtin_string = { "String" };
	NodeStructs::Type builtin_void = { "Void" };
};

static constexpr auto default_includes = 
	"#include <utility>\n"
	"#include <iostream>\n"

	"using Int = int;\n"
	"using Bool = bool;\n"
	"using Void = void;\n"
	"template <typename First, typename Second> using Pair = std::pair<First, Second>;\n"

	"#include <variant>\n"
	"template <typename... Ts> using Variant = std::variant<Ts...>;\n"

	"#include <vector>\n"
	"template <typename T> using Vector = std::vector<T>;\n"

	"#include <string>\n"
	"using String = std::string;\n"

	"#include <unordered_set>\n"
	"template <typename T> using Set = std::unordered_set<T>;\n"

	"#include <set>\n"
	"template <typename T> using TreeSet = std::set<T>;\n"

	"#include <unordered_map>\n"
	"template <typename K, typename V> using Map = std::unordered_map<K, V>;\n"

	"static constexpr bool True = true;\n"
	"static constexpr bool False = false;\n"

	"template<typename... Ts> struct overload : Ts... { using Ts::operator()...; };\n"

	"Void push(auto&& vec, auto&& e) { vec.push_back(e); }\n"
	/*"Void insert(auto&& set, auto&& e) { set.insert(e); }\n"
	"Void insert(auto&& map, auto&& k, auto&& v) { map.insert(k, v); }\n"*/

	"\n";

static std::string indent(size_t n) {
	std::string res;
	res.reserve(n);
	for (size_t i = 0; i < n; ++i)
		res += '\t';
	return res;
};

static NodeStructs::ValueCategory argument_category_to_value_category(const NodeStructs::ArgumentCategory cat) {
	return std::visit(overload(
		[](const NodeStructs::Reference&) -> NodeStructs::ValueCategory {
			return NodeStructs::Reference{};
		},
		[](const NodeStructs::MutableReference&) -> NodeStructs::ValueCategory {
			return NodeStructs::MutableReference{};
		},
		[](const NodeStructs::Copy&) -> NodeStructs::ValueCategory {
			return NodeStructs::Value{};
		},
		[](const NodeStructs::Move&) -> NodeStructs::ValueCategory {
			return NodeStructs::Value{};
		}
	), cat);
}

static NodeStructs::ValueCategory argument_category_optional_to_value_category(const std::optional<NodeStructs::ArgumentCategory> cat) {
	if (cat.has_value())
		return argument_category_to_value_category(cat.value());
	else
		throw;
}

transpile_header_cpp_t transpile(const std::vector<NodeStructs::File>& project);

transpile_header_cpp_t transpile_main(
	transpilation_state_with_indent state,
	const NodeStructs::Function& fn
);

transpile_header_cpp_t transpile(
	transpilation_state_with_indent state,
	const NodeStructs::Function& fn
);

transpile_header_cpp_t transpile(
	transpilation_state_with_indent state,
	const NodeStructs::Type& type
);

transpile_header_cpp_t transpile(
	transpilation_state_with_indent state,
	const NodeStructs::Interface& interface
);

transpile_t transpile(
	transpilation_state_with_indent state,
	const std::vector<std::tuple<NodeStructs::Typename, NodeStructs::ParameterCategory, std::string>>& parameters
);

transpile_t transpile(
	transpilation_state_with_indent state,
	const std::vector<NodeStructs::Statement>& statements
);

std::vector<NodeStructs::UniversalType> decomposed_type(
	transpilation_state_with_indent state,
	const NodeStructs::UniversalType& type
);

std::optional<error> add_decomposed_for_iterator_variables(
	transpilation_state_with_indent state,
	const std::vector<std::variant<NodeStructs::VariableDeclaration, std::string>>& iterators,
	const NodeStructs::UniversalType& it_type
);

std::optional<error> add_for_iterator_variable(
	transpilation_state_with_indent state,
	const std::vector<std::variant<NodeStructs::VariableDeclaration, std::string>>& iterators,
	const NodeStructs::UniversalType& it_type
);

void remove_for_iterator_variables(
	transpilation_state_with_indent state,
	const NodeStructs::ForStatement& statement
);

void remove_added_variables(
	transpilation_state_with_indent state,
	const NodeStructs::Statement& statement
);

transpile_t transpile_arg(
	transpilation_state_with_indent state,
	const NodeStructs::FunctionArgument& arg
);

transpile_t transpile_args(
	transpilation_state_with_indent state,
	const std::vector<NodeStructs::FunctionArgument>& args
);

transpile_t transpile_expressions(
	transpilation_state_with_indent state,
	const std::vector<NodeStructs::Expression>& args
);

transpile_t transpile_types(
	transpilation_state_with_indent state,
	const std::vector<NodeStructs::UniversalType>& args
);

NodeStructs::UniversalType iterator_type(
	transpilation_state_with_indent state,
	const NodeStructs::UniversalType& type
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
	const NodeStructs::UniversalType& expected_union_or_interface,
	const NodeStructs::UniversalType& observed_type
);

transpile_t expr_to_printable(
	transpilation_state_with_indent state,
	const NodeStructs::Expression& expr
);

transpile_t transpile_type(
	transpilation_state_with_indent state,
	const NodeStructs::UniversalType& type
);

std::optional<error> traverse_type(
	transpilation_state_with_indent state,
	const NodeStructs::UniversalType& type
);

expected<std::pair<NodeStructs::ParameterCategory, NodeStructs::UniversalType>> type_of_function_like_call_with_args(
	transpilation_state_with_indent state,
	const std::vector<NodeStructs::FunctionArgument>& arguments,
	const NodeStructs::UniversalType& type
);

expected<std::pair<NodeStructs::ParameterCategory, NodeStructs::UniversalType>> type_of_postfix_member(
	transpilation_state_with_indent state,
	const std::string& property_name,
	const NodeStructs::UniversalType& type
);

std::string expression_for_template(
	const NodeStructs::Expression& expr
);

transpile_t transpile_call_expression_with_args(
	transpilation_state_with_indent state,
	const std::vector<NodeStructs::FunctionArgument>& arguments,
	const NodeStructs::Expression& expr
);

transpile_t transpile_expression(
	transpilation_state_with_indent state,
	const NodeStructs::Expression& expr
);

expected<std::pair<NodeStructs::ValueCategory, NodeStructs::UniversalType>> type_of_expression(
	transpilation_state_with_indent state,
	const NodeStructs::Expression& expr
);

transpile_t transpile_statement(
	transpilation_state_with_indent state,
	const NodeStructs::Statement& statement
);

transpile_t transpile_typename(
	transpilation_state_with_indent state,
	const NodeStructs::Typename& tn
);

expected<NodeStructs::UniversalType> type_of_typename(
	transpilation_state_with_indent state,
	const NodeStructs::Typename& tn
);

expected<NodeStructs::UniversalType> type_template_of_typename(
	transpilation_state_with_indent state,
	const std::vector<NodeStructs::Typename>& templated_with,
	const NodeStructs::Typename& tn
);
