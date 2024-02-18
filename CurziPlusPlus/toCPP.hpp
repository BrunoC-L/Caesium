#pragma once
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <set>
#include "expected.hpp"
#include <ranges>

#include "no_copy.hpp"
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
private:
	no_copy _;
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

struct cpp_std {
	NodeStructs::Template builtin_set = { "Set" , { { "T", {} } }, "BUILTIN" };
	NodeStructs::Template builtin_vector = { "Vector" , { { "T", std::nullopt}}, "BUILTIN"};
	NodeStructs::Template builtin_map = { "Map" , { { "K", {} }, { "V", {} } }, "BUILTIN" };
	NodeStructs::Template builtin_push = { "push" , { { "Cnt", {} }, { "T", {} } }, "BUILTIN" }; // vec
	NodeStructs::Template builtin_insert = { "insert" , { { "Cnt", {} }, { "T", {} } }, "BUILTIN" }; // map or set
	NodeStructs::Type builtin_int = { "Int" };
	NodeStructs::Type builtin_bool = { "Bool" };
	NodeStructs::Type builtin_string = { "String" };
	NodeStructs::Type builtin_void = { "Void" };

	NodeStructs::Function println{
		.name = std::string{ "println" },
		.returnType = NodeStructs::Typename{ NodeStructs::BaseTypename{ "Void" } },
		.parameters = std::vector<std::tuple<NodeStructs::Typename, NodeStructs::ParameterCategory, std::string>>{
			{ NodeStructs::Typename{ NodeStructs::BaseTypename{ "String" } }, NodeStructs::ParameterCategory{ NodeStructs::Reference{} }, "t" }
		},
	};

	NodeStructs::Function size{
		.name = std::string{ "size" },
		.returnType = NodeStructs::Typename{ NodeStructs::BaseTypename{ "Int" } },
		.parameters = std::vector<std::tuple<NodeStructs::Typename, NodeStructs::ParameterCategory, std::string>>{
			{ NodeStructs::Typename{ NodeStructs::BaseTypename{ "T" } }, NodeStructs::ParameterCategory{ NodeStructs::Reference{} }, "t" }
		},
	};
};

static constexpr auto default_includes = 
	"#include <utility>\n"

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

	"\n";

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

static std::string indent(size_t n) {
	std::string res;
	res.reserve(n);
	for (size_t i = 0; i < n; ++i)
		res += '\t';
	return res;
};

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
