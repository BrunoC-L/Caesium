#pragma once
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <expected>
#include <ranges>

#include "node_structs.hpp"

struct user_error {
	std::string content;
	std::unexpected<user_error> unexpected() && {
		return std::unexpected<user_error>{ std::move(*this) };
	}
};
using variables_t = std::map<std::string, std::vector<std::pair<NodeStructs::ValueCategory, NodeStructs::TypeCategory>>>;
using transpile_t = std::expected<std::string, user_error>;
struct type_and_representation {
	NodeStructs::TypeCategory type;
	std::string representation;
};
using transpile_type_repr = std::expected<type_and_representation, user_error>;

template <size_t token>
std::string _symbol_as_text() {
	if constexpr (token == TOKENS::SEMICOLON) return ";";
	if constexpr (token == TOKENS::COLON || token == TOKENS::IN) return ":";
	if constexpr (token == TOKENS::NS) return "::";
	if constexpr (token == TOKENS::BACKSLASH) return "\\";
	if constexpr (token == TOKENS::DOT) return ".";
	if constexpr (token == TOKENS::COMMA) return ",";
	if constexpr (token == TOKENS::SPACE) return " ";
	if constexpr (token == TOKENS::TAB) return "\t";
	if constexpr (token == TOKENS::NEWLINE) return "\n";
	if constexpr (token == TOKENS::BRACEOPEN) return "{";
	if constexpr (token == TOKENS::BRACECLOSE) return "}";
	if constexpr (token == TOKENS::BRACKETOPEN) return "[";
	if constexpr (token == TOKENS::BRACKETCLOSE) return "]";
	if constexpr (token == TOKENS::PARENOPEN) return "(";
	if constexpr (token == TOKENS::PARENCLOSE) return ")";
	if constexpr (token == TOKENS::ASTERISK) return "*";
	if constexpr (token == TOKENS::SLASH) return "/";
	if constexpr (token == TOKENS::PERCENT) return "%";
	if constexpr (token == TOKENS::AMPERSAND) return "&";
	if constexpr (token == TOKENS::QUESTION) return "?";
	if constexpr (token == TOKENS::POUND) return "#";
	if constexpr (token == TOKENS::EQUAL) return "=";
	if constexpr (token == TOKENS::LT) return "<";
	if constexpr (token == TOKENS::GT) return ">";
	if constexpr (token == TOKENS::DASH) return "-";
	if constexpr (token == TOKENS::NOT) return "!";
	if constexpr (token == TOKENS::CARET) return "^";
	if constexpr (token == TOKENS::BITOR) return "|";
	if constexpr (token == TOKENS::PLUS) return "+";
	if constexpr (token == TOKENS::TILDE) return "~";
	if constexpr (token == TOKENS::PLUSPLUS) return "++";
	if constexpr (token == TOKENS::MINUSMINUS) return "--";
	if constexpr (token == TOKENS::EQUALEQUAL) return "==";
	if constexpr (token == TOKENS::NEQUAL) return "!=";
	if constexpr (token == TOKENS::PLUSEQUAL) return "+=";
	if constexpr (token == TOKENS::MINUSEQUAL) return "-=";
	if constexpr (token == TOKENS::TIMESEQUAL) return "*=";
	if constexpr (token == TOKENS::DIVEQUAL) return "/=";
	if constexpr (token == TOKENS::MODEQUAL) return "%=";
	if constexpr (token == TOKENS::ANDEQUAL) return "&=";
	if constexpr (token == TOKENS::OREQUAL) return "|=";
	if constexpr (token == TOKENS::LTE) return "<=";
	if constexpr (token == TOKENS::GTE) return ">=";
	if constexpr (token == TOKENS::XOREQUAL) return "^=";
	if constexpr (token == TOKENS::AND || token == TOKENS::ANDAND) return "&&";
	if constexpr (token == TOKENS::OR || token == TOKENS::OROR) return "||";
	if constexpr (token == TOKENS::RETURN) return "return";
	if constexpr (token == TOKENS::SWITCH) return "switch";
	if constexpr (token == TOKENS::FOR) return "for";
	if constexpr (token == TOKENS::WHILE) return "while";
	if constexpr (token == TOKENS::IF) return "if";
	if constexpr (token == TOKENS::ELSE) return "else";
	if constexpr (token == TOKENS::BREAK) return "break";
	if constexpr (token == TOKENS::CASE) return "case";
	if constexpr (token == TOKENS::DO) return "do";
	if constexpr (token == TOKENS::USING) return "using";
	if constexpr (token == TOKENS::STATIC) return "static";
	if constexpr (token == TOKENS::TYPE) return "struct";
	if constexpr (token == TOKENS::TEMPLATE) return "template";
	if constexpr (token == TOKENS::AUTO) return "auto";
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

struct Named {
	template <typename T> using map2vec = std::map<std::string, std::vector<std::reference_wrapper<const T>>>;
	map2vec<NodeStructs::Function> functions;
	map2vec<NodeStructs::Template<NodeStructs::Function>> function_templates;
	map2vec<NodeStructs::Type> types;
	map2vec<NodeStructs::Template<NodeStructs::Type>> type_templates;
	map2vec<NodeStructs::Block> blocks;
	map2vec<NodeStructs::Template<NodeStructs::Block>> block_templates;
};

struct cpp_std {
	NodeStructs::Type _uset = NodeStructs::Type{
		.name = std::string{ "Set" },
		.methods = std::vector<NodeStructs::Function>{},
		.memberVariables = std::vector<NodeStructs::MemberVariable>{},
	};

	NodeStructs::Template<NodeStructs::Type> unordered_set = {
		std::vector{ std::string{ "T" } },
		_uset,
	};

	NodeStructs::Type _vec = NodeStructs::Type{
		.name = std::string{ "Vector" },
		.methods = std::vector<NodeStructs::Function>{},
		.memberVariables = std::vector<NodeStructs::MemberVariable>{},
	};

	NodeStructs::Template<NodeStructs::Type> vector = {
		std::vector{std::string{"T"}},
		_vec,
	};

	NodeStructs::Template<NodeStructs::Type> unordered_map = []() {
		std::vector<NodeStructs::Function> methods = {
			NodeStructs::Function{
				"at",
				NodeStructs::BaseTypename{ "V" },
				std::vector<std::tuple<NodeStructs::Typename, NodeStructs::ValueCategory, std::string>>{},
				std::vector<NodeStructs::Statement>{},
			}
		};
		return NodeStructs::Template<NodeStructs::Type>{
			std::vector<std::string>{ "K", "V" },
			NodeStructs::Type{
				.name = std::string{ "Map" },
				.methods = std::move(methods),
			},
		};
	}();

	NodeStructs::Template<NodeStructs::Type> pair = {
		std::vector<std::string>{ "First", "Second" },
		NodeStructs::Type{
			.name = std::string{ "Pair" },
			.methods = std::vector<NodeStructs::Function>{},
			.memberVariables = std::vector<NodeStructs::MemberVariable>{},
		},
	};
	NodeStructs::Type _int = { "Int" };
	NodeStructs::Type _bool = { "Bool" };
	NodeStructs::Type string = { "String" };
	NodeStructs::Type _void = { "Void" };

	NodeStructs::Function println{
		.name = std::string{ "println" },
		.returnType = NodeStructs::Typename{ NodeStructs::BaseTypename{ "Void" } },
		.parameters = std::vector<std::tuple<NodeStructs::Typename, NodeStructs::ValueCategory, std::string>>{
			{ NodeStructs::Typename{ NodeStructs::BaseTypename{"T"} }, NodeStructs::ValueCategory{ NodeStructs::Value{} }, "t" }
		},
	};

	/*NodeStructs::Template<NodeStructs::Function> println = {
		std::vector<std::string>{ "T" },
		_println
	};*/
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

void insert_all_named_recursive_with_imports(const std::vector<NodeStructs::File>& project, Named& named, const std::string& filename);
std::expected<std::pair<std::string, std::string>, user_error> transpile(const std::vector<NodeStructs::File>& project);

transpile_t transpile_main_definition(
	variables_t& variables,
	const Named& named,
	const NodeStructs::Function& fn
);

transpile_t transpile_declaration(
	variables_t& variables,
	const Named& named,
	const NodeStructs::Function& fn
);

transpile_t transpile_definition(
	variables_t& variables,
	const Named& named,
	const NodeStructs::Function& fn
);

transpile_t transpile_declaration(
	variables_t& variables,
	const Named& named,
	const NodeStructs::Type& type
);

transpile_t transpile_definition(
	variables_t& variables,
	const Named& named,
	const NodeStructs::Type& type
);

template<typename T>
transpile_t transpile_declaration(
	variables_t& variables,
	const Named& named,
	const NodeStructs::Template<T>& tmpl
);

template<typename T>
transpile_t transpile_definition(
	variables_t& variables,
	const Named& named,
	const NodeStructs::Template<T>& tmpl
);

transpile_t transpile(
	variables_t& variables,
	const Named& named,
	const std::vector<std::tuple<NodeStructs::Typename, NodeStructs::ValueCategory, std::string>>& parameters
);

transpile_t transpile(
	variables_t& variables,
	const Named& named,
	const std::vector<NodeStructs::Statement>& statements
);

std::vector<NodeStructs::TypeCategory> decomposed_type(
	variables_t& variables,
	const Named& named,
	const NodeStructs::TypeCategory& type
);

std::optional<user_error> add_decomposed_for_iterator_variables(
	variables_t& variables,
	const Named& named,
	const std::vector<std::variant<NodeStructs::VariableDeclaration, std::string>>& iterators,
	const NodeStructs::TypeCategory& it_type
);

std::optional<user_error> add_for_iterator_variable(
	variables_t& variables,
	const Named& named,
	const std::vector<std::variant<NodeStructs::VariableDeclaration, std::string>>& iterators,
	const NodeStructs::TypeCategory& it_type
);

void remove_for_iterator_variables(
	variables_t& variables,
	const NodeStructs::ForStatement& statement
);

void remove_added_variables(
	variables_t& variables,
	const Named& named,
	const NodeStructs::Statement& statement
);

transpile_t transpile_arg(
	variables_t& variables,
	const Named& named,
	const NodeStructs::FunctionArgument& arg
);

transpile_t transpile_args(
	variables_t& variables,
	const Named& named,
	const std::vector<NodeStructs::FunctionArgument>& args
);

NodeStructs::TypeCategory iterator_type(
	variables_t& variables,
	const Named& named,
	const NodeStructs::TypeCategory& type
);
