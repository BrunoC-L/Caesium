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
	NodeStructs::ValueType type;
};
using variables_t = std::map<std::string, std::vector<variable_info>>;
using transpile_header_cpp_t = expected<std::pair<std::string, std::string>>;

struct type_information {
	NodeStructs::MetaType type;
	std::string representation;
};

struct primitive_information {
	NodeStructs::PrimitiveType type;
	std::string representation;
};

struct non_primitive_information {
	NodeStructs::NonPrimitiveType type;
	std::string representation;
	NodeStructs::ValueCategory value_category;
};

using expression_information = std::variant<type_information, primitive_information, non_primitive_information>;
using transpile_t = expected<std::string>;
using transpile_t2 = expected<expression_information>;

struct Named {
	template <typename T> using map_to_vec = std::map<std::string, std::vector<T const*>>;

	map_to_vec<NodeStructs::Function> functions;
	map_to_vec<NodeStructs::Function> functions_using_auto;
	map_to_vec<NodeStructs::Type> types;
	map_to_vec<NodeStructs::Interface> interfaces;
	map_to_vec<NodeStructs::Block> blocks;
	map_to_vec<NodeStructs::Template> templates;
	map_to_vec<NodeStructs::NameSpace> namespaces;
	std::map<std::string, NodeStructs::Typename> type_aliases_typenames;
	std::map<std::string, NodeStructs::MetaType> type_aliases;
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
	std::map<NodeStructs::Typename, std::vector<NodeStructs::MetaType>> interface_symbol_to_members;
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
	if constexpr (token == TOKENS::NOT) return "!";
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
	NodeStructs::Template builtin_set = { "Set", std::nullopt, { { "T", {} } }, "BUILTIN" };
	NodeStructs::Template builtin_vector = { "Vector", std::nullopt, { { "T", std::nullopt}}, "BUILTIN"};
	NodeStructs::Template builtin_map = { "Map", std::nullopt, { { "K", {} }, { "V", {} } }, "BUILTIN" };
	NodeStructs::Template builtin_push = { "push", std::nullopt, { { "Cnt", {} }, { "T", {} } }, "BUILTIN" }; // vec
	NodeStructs::Template builtin_insert = { "insert", std::nullopt, { { "Cnt", {} }, { "T", {} } }, "BUILTIN" }; // map or set

	NodeStructs::Template builtin_print = { "print", std::nullopt, { { "T", {} } }, "BUILTIN" };
	NodeStructs::Template builtin_println = { "println", std::nullopt, { { "T", {} } }, "BUILTIN" };

	NodeStructs::Type builtin_int = { "Int", std::nullopt };
	NodeStructs::Type builtin_bool = { "Bool", std::nullopt };
	NodeStructs::Type builtin_string = { "String", std::nullopt };
	NodeStructs::Type builtin_void = { "Void", std::nullopt };
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
		return NodeStructs::Value{};
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
	const std::vector<NodeStructs::FunctionParameter>& parameters
);

transpile_t transpile(
	transpilation_state_with_indent state,
	const std::vector<NodeStructs::Statement>& statements
);

std::vector<NodeStructs::MetaType> decompose_type(
	transpilation_state_with_indent state,
	const NodeStructs::MetaType& type
);

std::optional<error> add_decomposed_for_iterator_variables(
	transpilation_state_with_indent state,
	const std::vector<std::variant<NodeStructs::VariableDeclaration, std::string>>& iterators,
	const NodeStructs::MetaType& it_type
);

std::optional<error> add_for_iterator_variable(
	transpilation_state_with_indent state,
	const std::vector<std::variant<NodeStructs::VariableDeclaration, std::string>>& iterators,
	const NodeStructs::MetaType& it_type
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
	const std::vector<NodeStructs::MetaType>& args
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
#include "../expression_visitor/transpile_call_expression_with_args.hpp"
#include "../expression_visitor/transpile_expression_visitor.hpp"

#include "../statement_visitor/transpile_statement_visitor.hpp"

#include "../typename_visitor/transpile_typename_visitor.hpp"
#include "../typename_visitor/type_of_typename_visitor.hpp"
#include "../typename_visitor/type_template_of_typename_visitor.hpp"


expected<NodeStructs::Function> realise_function_using_auto(
	transpilation_state_with_indent state,
	const NodeStructs::Function& fn_using_auto,
	const std::vector<NodeStructs::FunctionArgument>& args
);